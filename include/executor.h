#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "lexer.h"

typedef enum {
    PARADO,
    RODANDO,
    INTERROMPIDO
} EstadoExecucao;

typedef struct {
    EstadoExecucao estado;
    char diretorioBase[1024];
} Contexto;

int executar(Instrucao *instrucoes, int total);

#endif