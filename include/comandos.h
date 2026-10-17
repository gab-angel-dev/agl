#ifndef COMANDOS_H
#define COMANDOS_H

#include "lexer.h"
#include "executor.h"

// Contrato: retorna 0 = ok, 1 = erro (já reportado ao usuário).
int cmd_create(const Instrucao *ins, Contexto *ctx);
int cmd_venv(const Instrucao *ins, Contexto *ctx);
int cmd_git(const Instrucao *ins, Contexto *ctx);
int cmd_bash(const Instrucao *ins, Contexto *ctx);
int cmd_print(const Instrucao *ins, Contexto *ctx);

#endif