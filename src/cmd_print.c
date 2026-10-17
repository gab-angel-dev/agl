#include <stdio.h>
#include "comandos.h"
#include "util.h"

// print "texto"
int cmd_print(const Instrucao *ins, Contexto *ctx) {
    (void)ctx;

    if (ins->quantidade != 2 || ins->tokens[1].tipo != TOKEN_STRING) {
        return agl_erro(ins->numLinha, "uso: print \"texto\"");
    }

    printf("%s\n", ins->tokens[1].valor);
    fflush(stdout);
    return 0;
}