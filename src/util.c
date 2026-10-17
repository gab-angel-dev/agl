#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "util.h"

#ifndef _WIN32
    #include <sys/wait.h>
#endif

#ifndef S_ISDIR
    #define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif

int agl_erro(int numLinha, const char *fmt, ...) {
    fflush(stdout);

    if (numLinha > 0) {
        fprintf(stderr, "Erro: linha %d: ", numLinha);
    } else {
        fprintf(stderr, "Erro: ");
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
    return 1;
}

char *copiar_string(const char *s) {
    size_t n = strlen(s);
    char *copia = malloc(n + 1);
    if (copia == NULL) {
        return NULL;
    }
    memcpy(copia, s, n + 1);
    return copia;
}

/* ---------- caminhos ---------- */

int eh_absoluto(const char *caminho) {
    if (caminho[0] == '/') {
        return 1;
    }
#ifdef _WIN32
    if (caminho[0] == '\\') {
        return 1;
    }
    if (isalpha((unsigned char)caminho[0]) && caminho[1] == ':') {
        return 1;
    }
#endif
    return 0;
}

int juntar_caminho(char *dest, size_t tam, const char *base, const char *rel) {
    int n;

    if (rel == NULL || rel[0] == '\0') {
        n = snprintf(dest, tam, "%s", base);
    } else if (eh_absoluto(rel)) {
        n = snprintf(dest, tam, "%s", rel);
    } else {
        size_t lb = strlen(base);
        int tem_sep = lb > 0 && EH_SEP(base[lb - 1]);
        n = snprintf(dest, tam, "%s%s%s", base, tem_sep ? "" : "/", rel);
    }

    return (n < 0 || (size_t)n >= tam) ? 1 : 0;
}

int existe(const char *caminho) {
    struct stat st;
    return stat(caminho, &st) == 0;
}

int eh_diretorio(const char *caminho) {
    struct stat st;
    return stat(caminho, &st) == 0 && S_ISDIR(st.st_mode);
}

int criar_dir_recursivo(const char *caminho) {
    char tmp[CAMINHO_MAX];
    size_t n = strlen(caminho);

    if (n == 0 || n >= sizeof(tmp)) {
        return 1;
    }
    memcpy(tmp, caminho, n + 1);

    // remove barras finais ("src/projeto/" -> "src/projeto")
    while (n > 1 && EH_SEP(tmp[n - 1])) {
        tmp[--n] = '\0';
    }

    // cria cada prefixo (começa em 1 para preservar a raiz "/")
    for (size_t i = 1; i < n; i++) {
        if (EH_SEP(tmp[i])) {
            char original = tmp[i];
            tmp[i] = '\0';
            if (MKDIR(tmp) != 0 && !eh_diretorio(tmp)) {
                return 1;
            }
            tmp[i] = original;
        }
    }

    if (MKDIR(tmp) != 0 && !eh_diretorio(tmp)) {
        return 1;
    }
    return 0;
}

int criar_pai(const char *caminho) {
    char tmp[CAMINHO_MAX];
    size_t n = strlen(caminho);

    if (n == 0 || n >= sizeof(tmp)) {
        return 1;
    }
    memcpy(tmp, caminho, n + 1);

    for (size_t i = n; i > 0; i--) {
        if (EH_SEP(tmp[i - 1])) {
            if (i - 1 == 0) {
                return 0;           // pai é a raiz
            }
            tmp[i - 1] = '\0';
            return criar_dir_recursivo(tmp);
        }
    }
    return 0;                       // sem pasta pai no caminho
}

/* ---------- 'in' ---------- */

static int eh_palavra(const Token *t, const char *texto) {
    return t->tipo == TOKEN_PALAVRA && strcmp(t->valor, texto) == 0;
}

int extrair_in(const Instrucao *ins, int *fim, const char **destino) {
    int q = ins->quantidade;
    *fim = q;
    *destino = NULL;

    if (q >= 1 && eh_palavra(&ins->tokens[q - 1], "in")) {
        return agl_erro(ins->numLinha, "'in' precisa ser seguido de um caminho");
    }

    if (q >= 2 && eh_palavra(&ins->tokens[q - 2], "in")) {
        const Token *t = &ins->tokens[q - 1];
        if (t->tipo != TOKEN_PALAVRA && t->tipo != TOKEN_STRING) {
            return agl_erro(ins->numLinha, "caminho inválido depois de 'in'");
        }
        *destino = t->valor;
        *fim = q - 2;
    }

    return 0;
}

/* ---------- shell ---------- */

char *shell_quote(const char *s) {
    size_t n = strlen(s);
    char *out = malloc(n * 4 + 3);
    if (out == NULL) {
        return NULL;
    }
    char *p = out;

#ifdef _WIN32
    *p++ = '"';
    memcpy(p, s, n);
    p += n;
    *p++ = '"';
#else
    *p++ = '\'';
    for (; *s; s++) {
        if (*s == '\'') {
            memcpy(p, "'\\''", 4);
            p += 4;
        } else {
            *p++ = *s;
        }
    }
    *p++ = '\'';
#endif

    *p = '\0';
    return out;
}

int executar_shell(const char *dir, const char *venv, const char *comando) {
    size_t cap = strlen(dir) * 4 + (venv ? strlen(venv) * 8 : 0) + strlen(comando) + 256;
    char *cmd = malloc(cap);
    char *qdir = shell_quote(dir);
    char *qvenv = NULL;
    char *qbin = NULL;
    int rc = -1;

    if (cmd == NULL || qdir == NULL) {
        goto fim;
    }

    int n;
#ifdef _WIN32
    n = snprintf(cmd, cap, "cd /d %s", qdir);
    if (venv) {
        n += snprintf(cmd + n, cap - n,
                      " && set \"VIRTUAL_ENV=%s\" && set \"PATH=%s\\Scripts;%%PATH%%\"", venv, venv);
    }
    snprintf(cmd + n, cap - n, " && %s", comando);
#else
    n = snprintf(cmd, cap, "cd %s", qdir);
    if (venv) {
        char bin[CAMINHO_MAX + 8];
        snprintf(bin, sizeof(bin), "%s/bin", venv);
        qvenv = shell_quote(venv);
        qbin = shell_quote(bin);
        if (qvenv == NULL || qbin == NULL) {
            goto fim;
        }
        n += snprintf(cmd + n, cap - n,
                      " && export VIRTUAL_ENV=%s && export PATH=%s:\"$PATH\"", qvenv, qbin);
    }
    // subshell com quebra de linha: aceita ';', '&&', pipes e comentário no final
    snprintf(cmd + n, cap - n, " && (\n%s\n)", comando);
#endif

    fflush(stdout);
    fflush(stderr);

    int status = system(cmd);
    if (status == -1) {
        rc = -1;
    } else {
#ifdef _WIN32
        rc = status;
#else
        rc = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
    }

fim:
    free(cmd);
    free(qdir);
    free(qvenv);
    free(qbin);
    return rc;
}