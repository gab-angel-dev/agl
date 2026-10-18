#include <stdio.h>
#include <string.h>
#include "templates.h"

#define PREFIXO_DESCRICAO "# description:"

const Template *buscar_template(const char *nome) {
    for (int i = 0; i < TOTAL_TEMPLATES; i++) {
        if (strcmp(nome, TEMPLATES[i].nome) == 0) {
            return &TEMPLATES[i];
        }
    }
    return NULL;
}

const char *descricao_template(const Template *t, char *buf, size_t tam) {
    buf[0] = '\0';

    const char *linha = t->conteudo;
    size_t prefixo = strlen(PREFIXO_DESCRICAO);

    while (*linha != '\0') {
        const char *fim = strchr(linha, '\n');
        size_t len = fim ? (size_t)(fim - linha) : strlen(linha);

        if (len >= prefixo && strncmp(linha, PREFIXO_DESCRICAO, prefixo) == 0) {
            const char *texto = linha + prefixo;
            len -= prefixo;
            while (len > 0 && *texto == ' ') {
                texto++;
                len--;
            }
            if (len >= tam) {
                len = tam - 1;
            }
            memcpy(buf, texto, len);
            buf[len] = '\0';
            return buf;
        }

        if (fim == NULL) {
            break;
        }
        linha = fim + 1;
    }

    return buf;
}

void listar_templates(void) {
    if (TOTAL_TEMPLATES == 0) {
        printf("Nenhum template disponível.\n");
        return;
    }

    size_t maior = 0;
    for (int i = 0; i < TOTAL_TEMPLATES; i++) {
        size_t n = strlen(TEMPLATES[i].nome);
        if (n > maior) {
            maior = n;
        }
    }

    printf("Templates disponíveis:\n\n");

    for (int i = 0; i < TOTAL_TEMPLATES; i++) {
        char desc[256];
        descricao_template(&TEMPLATES[i], desc, sizeof(desc));
        printf("  %s:%*s %s\n", TEMPLATES[i].nome,
               (int)(maior - strlen(TEMPLATES[i].nome)), "", desc);
    }

    printf("\nUso: agl run template <nome>\n");
}