#include <stdio.h>
#include <string.h>
#include "comandos.h"

int cmd_create(const Instrucao *ins, Contexto *ctx) {
    // tokens[0] = "create"
    // tokens[1] = "dir" | "file"
    // ...

    // TODO: validar quantidade de tokens
    // TODO: decidir dir ou file
    // TODO: executar

    (void)ins;   // silencia warning enquanto está vazio
    (void)ctx;
    return 0;
}