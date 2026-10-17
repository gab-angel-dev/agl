#include <stdio.h>
#include <string.h>
#include "comandos.h"
#include "util.h"

static const char *GITIGNORE_PADRAO =
    "# Python\n"
    "__pycache__/\n"
    "*.py[cod]\n"
    ".venv/\n"
    "venv/\n"
    ".env\n"
    "\n"
    "# C\n"
    "*.o\n"
    "*.d\n"
    "obj/\n"
    "\n"
    "# Node\n"
    "node_modules/\n"
    "\n"
    "# Sistema / editores\n"
    ".DS_Store\n"
    "Thumbs.db\n"
    ".idea/\n"
    ".vscode/\n";

static int git_ignr(const Instrucao *ins, const char *dir) {
    char caminho[CAMINHO_MAX];

    if (juntar_caminho(caminho, sizeof(caminho), dir, ".gitignore")) {
        return agl_erro(ins->numLinha, "caminho muito longo");
    }

    if (existe(caminho)) {
        printf("= file  %s\n", caminho);    // não sobrescreve
        return 0;
    }

    if (criar_dir_recursivo(dir) != 0) {
        return agl_erro(ins->numLinha, "não foi possível criar a pasta '%s'", dir);
    }

    FILE *f = fopen(caminho, "w");
    if (f == NULL) {
        return agl_erro(ins->numLinha, "não foi possível criar '%s'", caminho);
    }
    fputs(GITIGNORE_PADRAO, f);
    fclose(f);

    printf("+ file  %s\n", caminho);
    return 0;
}

// git init [in <caminho>]
// git ignr [in <caminho>]
int cmd_git(const Instrucao *ins, Contexto *ctx) {
    if (ins->quantidade < 2 || ins->tokens[1].tipo != TOKEN_PALAVRA) {
        return agl_erro(ins->numLinha, "uso: git init|ignr [in <caminho>]");
    }

    const char *sub = ins->tokens[1].valor;
    int eh_init = strcmp(sub, "init") == 0;
    int eh_ignr = strcmp(sub, "ignr") == 0;

    if (!eh_init && !eh_ignr) {
        return agl_erro(ins->numLinha, "'git %s' desconhecido (init, ignr)", sub);
    }

    int fim;
    const char *destino;
    if (extrair_in(ins, &fim, &destino) != 0) {
        return 1;
    }
    if (fim != 2) {
        return agl_erro(ins->numLinha, "uso: git %s [in <caminho>]", sub);
    }

    char dir[CAMINHO_MAX];
    if (juntar_caminho(dir, sizeof(dir), ctx->diretorioBase, destino)) {
        return agl_erro(ins->numLinha, "caminho muito longo");
    }

    if (eh_ignr) {
        return git_ignr(ins, dir);
    }

    if (criar_dir_recursivo(dir) != 0) {
        return agl_erro(ins->numLinha, "não foi possível criar a pasta '%s'", dir);
    }

    int rc = executar_shell(dir, NULL, "git init");
    if (rc != 0) {
        return agl_erro(ins->numLinha, "'git init' falhou (git instalado? código %d)", rc);
    }
    return 0;
}