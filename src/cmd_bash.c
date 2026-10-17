#include <stdio.h>
#include <string.h>
#include "comandos.h"
#include "util.h"

// bash > <comando de shell>
int cmd_bash(const Instrucao *ins, Contexto *ctx) {
    if (ins->quantidade != 3
        || ins->tokens[1].tipo != TOKEN_SIMBOLO || ins->tokens[1].valor[0] != '>'
        || ins->tokens[2].tipo != TOKEN_BRUTO
        || ins->tokens[2].valor[0] == '\0') {
        return agl_erro(ins->numLinha, "uso: bash > <comando>");
    }

    const char *venv = ctx->venvAtiva[0] ? ctx->venvAtiva : NULL;
    int rc = executar_shell(ctx->diretorioBase, venv, ins->tokens[2].valor);

    if (rc == -1) {
        return agl_erro(ins->numLinha, "não foi possível executar o comando");
    }
    if (rc != 0) {
        return agl_erro(ins->numLinha, "o comando terminou com código %d", rc);
    }
    return 0;
}