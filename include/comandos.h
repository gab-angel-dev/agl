#ifndef COMANDOS_H
#define COMANDOS_H

typedef enum {
    PARADO,
    RODANDO,
    INTERROMPIDO
} EstadoExecucao;


int identificar_comando(char *linha, EstadoExecucao *estado, int numLinha);
int executar_print(char *linha, EstadoExecucao *estado, int numLinha);

#endif