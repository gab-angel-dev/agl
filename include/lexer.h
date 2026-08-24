#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_PALAVRA,
    TOKEN_STRING,
    TOKEN_FIM_LINHA,
    TOKEN_DESCONHECIDO
} TipoToken;

typedef struct {
    TipoToken tipo;
    char *valor;
} Token;

#endif