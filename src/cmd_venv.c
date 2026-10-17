#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#include "util.h"

static int token_nome(const Token *t) {
    return t->tipo == TOKEN_PALAVRA || t->tipo == TOKEN_STRING;
}

// Monta <base>/<destino>/<nome>
static int montar_caminho(const Instrucao *ins, const Contexto *ctx,
                          const char *destino, const char *nome, char *saida, size_t tam) {
    char dir[CAMINHO_MAX];

    if (juntar_caminho(dir, sizeof(dir), ctx->diretorioBase, destino)
        || juntar_caminho(saida, tam, dir, nome)) {
        return agl_erro(ins->numLinha, "caminho muito longo");
    }
    return 0;
}

static int venv_create(const Instrucao *ins, Contexto *ctx, const char *nome, const char *destino) {
    char caminho[CAMINHO_MAX];
    if (montar_caminho(ins, ctx, destino, nome, caminho, sizeof(caminho))) {
        return 1;
    }

    char script[CAMINHO_MAX + 32];
    snprintf(script, sizeof(script), "%s%s", caminho, ACTIVATE_REL);

    if (existe(script)) {
        printf("= venv  %s\n", caminho);
    } else {
        char pai[CAMINHO_MAX];
        if (juntar_caminho(pai, sizeof(pai), ctx->diretorioBase, destino)
            || criar_dir_recursivo(pai) != 0) {
            return agl_erro(ins->numLinha, "não foi possível criar a pasta '%s'", pai);
        }

        char *q = shell_quote(caminho);
        if (q == NULL) {
            return agl_erro(ins->numLinha, "sem memória");
        }

        size_t cap = strlen(q) + 64;
        char *cmd = malloc(cap);
        if (cmd == NULL) {
            free(q);
            return agl_erro(ins->numLinha, "sem memória");
        }
        snprintf(cmd, cap, PYTHON_CMD " -m venv %s", q);

        int rc = executar_shell(ctx->diretorioBase, NULL, cmd);
        free(cmd);
        free(q);

        if (rc != 0) {
            return agl_erro(ins->numLinha, "falha ao criar a venv (" PYTHON_CMD " instalado? código %d)", rc);
        }
        printf("+ venv  %s\n", caminho);
    }

    snprintf(ctx->ultimaVenv, sizeof(ctx->ultimaVenv), "%s", caminho);
    return 0;
}

static int venv_active(const Instrucao *ins, Contexto *ctx, const char *nome, const char *destino) {
    char caminho[CAMINHO_MAX];

    if (nome == NULL && destino == NULL && ctx->ultimaVenv[0] != '\0') {
        snprintf(caminho, sizeof(caminho), "%s", ctx->ultimaVenv);
    } else {
        if (montar_caminho(ins, ctx, destino, nome ? nome : ".venv", caminho, sizeof(caminho))) {
            return 1;
        }
    }

    char script[CAMINHO_MAX + 32];
    snprintf(script, sizeof(script), "%s%s", caminho, ACTIVATE_REL);

    if (!existe(script)) {
        return agl_erro(ins->numLinha, "venv não encontrada em '%s' (use 'venv create' antes)", caminho);
    }

    snprintf(ctx->venvAtiva, sizeof(ctx->venvAtiva), "%s", caminho);
    printf("~ venv ativa: %s\n", caminho);
    return 0;
}

// venv create <nome> [in <caminho>]
// venv active [<nome>] [in <caminho>]
// venv deactive
int cmd_venv(const Instrucao *ins, Contexto *ctx) {
    const char *uso = "uso: venv create <nome> [in <caminho>] | venv active [<nome>] [in <caminho>] | venv deactive";

    if (ins->quantidade < 2 || ins->tokens[1].tipo != TOKEN_PALAVRA) {
        return agl_erro(ins->numLinha, "%s", uso);
    }

    const char *sub = ins->tokens[1].valor;

    if (strcmp(sub, "deactive") == 0) {
        if (ins->quantidade != 2) {
            return agl_erro(ins->numLinha, "uso: venv deactive");
        }
        if (ctx->venvAtiva[0] == '\0') {
            printf("~ nenhuma venv ativa\n");
        } else {
            ctx->venvAtiva[0] = '\0';
            printf("~ venv desativada\n");
        }
        return 0;
    }

    int eh_create = strcmp(sub, "create") == 0;
    int eh_active = strcmp(sub, "active") == 0;

    if (!eh_create && !eh_active) {
        return agl_erro(ins->numLinha, "'venv %s' desconhecido (create, active, deactive)", sub);
    }

    int fim;
    const char *destino;
    if (extrair_in(ins, &fim, &destino) != 0) {
        return 1;
    }

    const char *nome = NULL;
    if (fim == 3) {
        if (!token_nome(&ins->tokens[2])) {
            return agl_erro(ins->numLinha, "%s", uso);
        }
        nome = ins->tokens[2].valor;
    } else if (fim != 2) {
        return agl_erro(ins->numLinha, "%s", uso);
    }

    if (eh_create) {
        if (nome == NULL) {
            return agl_erro(ins->numLinha, "uso: venv create <nome> [in <caminho>]");
        }
        return venv_create(ins, ctx, nome, destino);
    }

    return venv_active(ins, ctx, nome, destino);
}