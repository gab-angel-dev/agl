#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>
#include "comandos.h"

void liberar_tokens(Token *tokens, int total){
    for (int i = 0; i < total; i++) {
        free(tokens[i].valor);
    }
    free(tokens);
}

char *extrair_ate(char *inicio, int tamanho){
    char *valor = malloc(tamanho * sizeof(char) + 1);

    if (valor == NULL){
        printf("Erro: sem memória\n");
        return NULL;
    }

    for (int i = 0; i < tamanho; i++){
        valor[i] = inicio[i];
    }

    valor[tamanho] = '\0';

    return valor;
}


Token *tokenizar(char *linha, int *quantidadeTokens, ErroLexer *erro) {
    *erro = ERRO_NENHUM;
    int capacidade = 4;
    int total = 0;

    Token *tokens = malloc(capacidade * sizeof(Token));
    if (tokens == NULL) {
        *erro = ERRO_MEMORIA;
        return NULL;
    }

    while (*linha != '\0' && *linha != '\n') {

        linha = pular_espacos(linha);

        if (*linha == '\0' || *linha == '\n') {
            break;
        }

        if (total == capacidade){
            capacidade *= 2;
            Token *temp = realloc(tokens, capacidade * sizeof(Token));

            if (temp == NULL){
                *erro = ERRO_MEMORIA;
                liberar_tokens(tokens, total);
                return NULL;
            }

            tokens = temp;
        }


        Token novoToken;

        if (*linha == '"') {
            linha++;
            int quantCaracteres = 0;
            while (linha[quantCaracteres] != '"' ){
                if (linha[quantCaracteres] == '\0' || linha[quantCaracteres] == '\n'){
                        *erro = ERRO_ASPAS_NAO_FECHADAS;
                        liberar_tokens(tokens, total);
                        return NULL;
                }
                quantCaracteres++;
            }

            char *texto = extrair_ate(linha, quantCaracteres);
            if (texto == NULL){
                *erro = ERRO_MEMORIA;
                liberar_tokens(tokens, total);
                return NULL;
            }

            novoToken.tipo = TOKEN_STRING;
            novoToken.valor = texto;

            linha += quantCaracteres + 1;

        }
        else {
            int quantCaracteres = 0;
            while (linha[quantCaracteres] != '\n' && linha[quantCaracteres] != '\0' && linha[quantCaracteres] != '"' && linha[quantCaracteres] != ' '){
                quantCaracteres++;
            }

            char *valor = extrair_ate(linha, quantCaracteres);
            if (valor == NULL){
                *erro = ERRO_MEMORIA;
                liberar_tokens(tokens, total);
                return NULL;
            }

            novoToken.tipo = TOKEN_PALAVRA;
            novoToken.valor = valor;

            linha += quantCaracteres;

        }

        tokens[total] = novoToken;
        total++;
    }

    *quantidadeTokens = total;
    return tokens;
}