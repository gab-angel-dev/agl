#ifndef COMANDOS_H
#define COMANDOS_H

#include "lexer.h"
#include "executor.h"

// Retorna 0 = ok, 1 = erro
int cmd_create(const Instrucao *ins, Contexto *ctx);
int cmd_print(const Instrucao *ins, Contexto *ctx);
int cmd_bash(const Instrucao *ins, Contexto *ctx);

#endif