#include <stdio.h>
#include <string.h>
#include "executor.h"
#include "comandos.h"

typedef struct {
    const char *nome;
    int (*funcao)(const Instrucao *, Contexto *);
} Comando;

static const Comando tabela[] = {
    {"create", cmd_create},
    {"print",  cmd_print},
    {"bash",   cmd_bash},
};

static const Comando *buscar_comando(const char *nome) {
    // TODO: percorrer a tabela com strcmp
    (void)nome;
    return NULL;
}

int executar(Instrucao *instrucoes, int total) {
    Contexto ctx = { PARADO, "." };

    for (int i = 0; i < total && ctx.estado != INTERROMPIDO; i++) {
        // TODO: pular instrução vazia
        // TODO: buscar o comando pelo tokens[0]
        // TODO: chamar a função
    }

    return ctx.estado == INTERROMPIDO ? 1 : 0;
}