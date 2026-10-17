#include <stdio.h>
#include <string.h>
#include "comandos.h"
#include "util.h"

static int criar_dir(int linha, const char *caminho) {
    int existia = eh_diretorio(caminho);

    if (criar_dir_recursivo(caminho) != 0) {
        return agl_erro(linha, "não foi possível criar a pasta '%s'", caminho);
    }

    printf("%s dir   %s\n", existia ? "=" : "+", caminho);
    return 0;
}

static int criar_arquivo(int linha, const char *caminho) {
    if (eh_diretorio(caminho)) {
        return agl_erro(linha, "'%s' já existe e é uma pasta", caminho);
    }

    int existia = existe(caminho);

    if (criar_pai(caminho) != 0) {
        return agl_erro(linha, "não foi possível criar as pastas de '%s'", caminho);
    }

    FILE *f = fopen(caminho, "a");      // "a": não apaga conteúdo existente
    if (f == NULL) {
        return agl_erro(linha, "não foi possível criar o arquivo '%s'", caminho);
    }
    fclose(f);

    printf("%s file  %s\n", existia ? "=" : "+", caminho);
    return 0;
}

static int token_nome(const Token *t) {
    return t->tipo == TOKEN_PALAVRA || t->tipo == TOKEN_STRING;
}

// create dir|file nome[, nome...] [in <caminho>]
int cmd_create(const Instrucao *ins, Contexto *ctx) {
    const char *uso = "uso: create dir|file <nome>[, <nome>...] [in <caminho>]";

    if (ins->quantidade < 3 || ins->tokens[1].tipo != TOKEN_PALAVRA) {
        return agl_erro(ins->numLinha, "%s", uso);
    }

    int eh_dir;
    if (strcmp(ins->tokens[1].valor, "dir") == 0) {
        eh_dir = 1;
    } else if (strcmp(ins->tokens[1].valor, "file") == 0) {
        eh_dir = 0;
    } else {
        return agl_erro(ins->numLinha, "'create %s' desconhecido (use dir ou file)", ins->tokens[1].valor);
    }

    int fim;
    const char *destino;
    if (extrair_in(ins, &fim, &destino) != 0) {
        return 1;
    }

    // itens em tokens[2 .. fim): nome , nome , nome  -> quantidade ímpar
    int n = fim - 2;
    if (n < 1 || n % 2 == 0) {
        return agl_erro(ins->numLinha, "%s", uso);
    }
    for (int k = 0; k < n; k++) {
        const Token *t = &ins->tokens[2 + k];
        int ok = (k % 2 == 0) ? token_nome(t)
                              : (t->tipo == TOKEN_SIMBOLO && t->valor[0] == ',');
        if (!ok) {
            return agl_erro(ins->numLinha, "%s", uso);
        }
    }

    char alvo[CAMINHO_MAX];
    if (juntar_caminho(alvo, sizeof(alvo), ctx->diretorioBase, destino)) {
        return agl_erro(ins->numLinha, "caminho muito longo");
    }

    for (int k = 0; k < n; k += 2) {
        char caminho[CAMINHO_MAX];
        if (juntar_caminho(caminho, sizeof(caminho), alvo, ins->tokens[2 + k].valor)) {
            return agl_erro(ins->numLinha, "caminho muito longo");
        }

        int rc = eh_dir ? criar_dir(ins->numLinha, caminho)
                        : criar_arquivo(ins->numLinha, caminho);
        if (rc != 0) {
            return 1;
        }
    }

    return 0;
}