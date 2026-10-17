#include <stdio.h>
#include <string.h>
#include "executor.h"
#include "comandos.h"
#include "util.h"

typedef struct {
    const char *nome;
    int (*funcao)(const Instrucao *, Contexto *);
} Comando;

// Para criar um comando novo: escreva cmd_xxx em src/cmd_xxx.c,
// declare em comandos.h e adicione uma linha aqui.
static const Comando tabela[] = {
    {"create", cmd_create},
    {"venv",   cmd_venv},
    {"git",    cmd_git},
    {"bash",   cmd_bash},
    {"print",  cmd_print},
};

static const Comando *buscar_comando(const char *nome) {
    size_t tamanho = sizeof(tabela) / sizeof(tabela[0]);

    for (size_t i = 0; i < tamanho; i++) {
        if (strcmp(nome, tabela[i].nome) == 0) {
            return &tabela[i];
        }
    }
    return NULL;
}

static int token_eh(const Token *t, TipoToken tipo, const char *texto) {
    return t->tipo == tipo && strcmp(t->valor, texto) == 0;
}

// $ start [into <caminho>]   |   $ stop
static int tratar_estado(const Instrucao *ins, Contexto *ctx) {
    if (ins->quantidade < 2 || ins->tokens[1].tipo != TOKEN_PALAVRA) {
        return agl_erro(ins->numLinha, "uso: $ start [into <caminho>] | $ stop");
    }

    const char *acao = ins->tokens[1].valor;

    if (strcmp(acao, "start") == 0) {
        if (ctx->estado == RODANDO) {
            return agl_erro(ins->numLinha, "'$ start' dentro de um bloco aberto (falta '$ stop')");
        }

        char base[CAMINHO_MAX];

        if (ins->quantidade == 2) {
            snprintf(base, sizeof(base), "%s", ctx->diretorioOriginal);
        }
        else if (ins->quantidade == 4 && token_eh(&ins->tokens[2], TOKEN_PALAVRA, "into")
                 && (ins->tokens[3].tipo == TOKEN_PALAVRA || ins->tokens[3].tipo == TOKEN_STRING)) {

            if (juntar_caminho(base, sizeof(base), ctx->diretorioOriginal, ins->tokens[3].valor)) {
                return agl_erro(ins->numLinha, "caminho muito longo");
            }
            if (criar_dir_recursivo(base) != 0) {
                return agl_erro(ins->numLinha, "não foi possível criar/acessar '%s'", base);
            }
        }
        else {
            return agl_erro(ins->numLinha, "uso: $ start [into <caminho>]");
        }

        snprintf(ctx->diretorioBase, sizeof(ctx->diretorioBase), "%s", base);
        ctx->estado = RODANDO;
        return 0;
    }

    if (strcmp(acao, "stop") == 0) {
        if (ins->quantidade != 2) {
            return agl_erro(ins->numLinha, "uso: $ stop");
        }
        if (ctx->estado != RODANDO) {
            return agl_erro(ins->numLinha, "'$ stop' sem '$ start'");
        }

        snprintf(ctx->diretorioBase, sizeof(ctx->diretorioBase), "%s", ctx->diretorioOriginal);
        ctx->venvAtiva[0] = '\0';
        ctx->estado = PARADO;
        return 0;
    }

    return agl_erro(ins->numLinha, "'$ %s' desconhecido (use start ou stop)", acao);
}

int executar(Instrucao *instrucoes, int total) {
    Contexto ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.estado = PARADO;

    if (getcwd(ctx.diretorioOriginal, sizeof(ctx.diretorioOriginal)) == NULL) {
        return agl_erro(0, "não foi possível obter o diretório atual");
    }
    snprintf(ctx.diretorioBase, sizeof(ctx.diretorioBase), "%s", ctx.diretorioOriginal);

    for (int i = 0; i < total; i++) {
        const Instrucao *ins = &instrucoes[i];

        if (ins->quantidade == 0) {
            continue;                       // linha vazia ou só comentário
        }

        const Token *primeiro = &ins->tokens[0];
        int resultado;

        if (primeiro->tipo == TOKEN_SIMBOLO && primeiro->valor[0] == '$') {
            resultado = tratar_estado(ins, &ctx);
        }
        else if (primeiro->tipo != TOKEN_PALAVRA) {
            resultado = agl_erro(ins->numLinha, "a linha deve começar com '$' ou com um comando");
        }
        else {
            const Comando *cmd = buscar_comando(primeiro->valor);

            if (cmd == NULL) {
                resultado = agl_erro(ins->numLinha, "comando '%s' desconhecido", primeiro->valor);
            }
            else if (ctx.estado != RODANDO) {
                resultado = agl_erro(ins->numLinha, "'%s' fora de um bloco: use '$ start' antes", primeiro->valor);
            }
            else {
                resultado = cmd->funcao(ins, &ctx);
            }
        }

        if (resultado != 0) {
            ctx.estado = INTERROMPIDO;
            break;
        }
    }

    if (ctx.estado == RODANDO) {
        fprintf(stderr, "Aviso: bloco '$ start' sem '$ stop' no fim do arquivo\n");
    }

    return ctx.estado == INTERROMPIDO ? 1 : 0;
}