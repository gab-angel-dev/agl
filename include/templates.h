#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <stddef.h>

typedef struct {
    const char *nome;       // nome do arquivo sem ".agl"
    const char *conteudo;   // texto do .agl embutido no binário
} Template;

// Definidos em obj/templates_data.c (gerado no build a partir de templates/*.agl)
extern const Template TEMPLATES[];
extern const int TOTAL_TEMPLATES;

const Template *buscar_template(const char *nome);

// Lê a linha "# descricao: ..." do template. Retorna buf (vazio se não houver).
const char *descricao_template(const Template *t, char *buf, size_t tam);

void listar_templates(void);

#endif