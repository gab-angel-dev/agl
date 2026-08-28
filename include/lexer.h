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

typedef struct {
    Token *tokens;
    int quantidade;
    int numLinha;
} Instrucao;

typedef enum {
    ERRO_NENHUM,
    ERRO_MEMORIA,
    ERRO_ASPAS_NAO_FECHADAS
} ErroLexer;

Token *tokenizar(char *linha, int *quantidadeTokens, ErroLexer *erro);
Instrucao *montar_instrucoes(char **linhas, int quantidadeLinhas, int *quantidadeInstrucoes, ErroLexer *erro);
void liberar_instrucoes(Instrucao *instrucoes, int total);

#endif