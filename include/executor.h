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
    char diretorioBase[512];
} Contexto;

int executar(Instrucao *instrucoes, int total);

#endif