#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <sys/stat.h>
#include "lexer.h"
#include "executor.h"

#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd
    #define MKDIR(p) _mkdir(p)
    #define EH_SEP(c) ((c) == '/' || (c) == '\\')
    #define PYTHON_CMD "python"
    #define ACTIVATE_REL "/Scripts/activate.bat"
#else
    #include <unistd.h>
    #define MKDIR(p) mkdir((p), 0777)
    #define EH_SEP(c) ((c) == '/')
    #define PYTHON_CMD "python3"
    #define ACTIVATE_REL "/bin/activate"
#endif

#ifdef __GNUC__
    #define AGL_PRINTF(a, b) __attribute__((format(printf, a, b)))
#else
    #define AGL_PRINTF(a, b)
#endif

// Erro padronizado em stderr. Sempre retorna 1 (permite: return agl_erro(...)).
int agl_erro(int numLinha, const char *fmt, ...) AGL_PRINTF(2, 3);

char *copiar_string(const char *s);

// Caminhos
int eh_absoluto(const char *caminho);
int juntar_caminho(char *dest, size_t tam, const char *base, const char *rel); // 0 ok, 1 estouro
int existe(const char *caminho);
int eh_diretorio(const char *caminho);
int criar_dir_recursivo(const char *caminho);   // mkdir -p ; 0 ok, 1 falha
int criar_pai(const char *caminho);             // cria as pastas pai de um arquivo

// Sintaxe "... in <caminho>": separa o 'in' do final da instrução.
// *fim = índice (exclusivo) onde terminam os argumentos; *destino = caminho ou NULL.
int extrair_in(const Instrucao *ins, int *fim, const char **destino);

// Shell
char *shell_quote(const char *s);
// Roda 'comando' em 'dir' (com venv opcional). Retorna código de saída, -1 se não conseguiu rodar.
int executar_shell(const char *dir, const char *venv, const char *comando);

#endif