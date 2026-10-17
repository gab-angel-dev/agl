#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "lexer.h"

#define CAMINHO_MAX 1024

typedef enum {
    PARADO,
    RODANDO,
    INTERROMPIDO
} EstadoExecucao;

typedef struct {
    EstadoExecucao estado;
    char diretorioOriginal[CAMINHO_MAX];  // cwd de quem rodou o 'agl run'
    char diretorioBase[CAMINHO_MAX];      // base do bloco atual ($ start [into X])
    char venvAtiva[CAMINHO_MAX];          // "" = nenhuma
    char ultimaVenv[CAMINHO_MAX];         // última criada por 'venv create'
} Contexto;

// Retorna 0 se tudo certo, 1 se houve erro.
int executar(Instrucao *instrucoes, int total);

#endif