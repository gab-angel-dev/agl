#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>
#include <ctype.h>

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


void liberar_tokens(Token *tokens, int total){
    for (int i = 0; i < total; i++) {
        free(tokens[i].valor);
    }
    free(tokens);
}

static char *pular_espacos(char *linha) {
    while (*linha != '\n' && isspace((unsigned char)*linha)) {
        linha++;
    }
    return linha;
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
        
        else if (*linha == '#'){
            *quantidadeTokens = total;
            return tokens;
        }

        else if (*linha == '$' || *linha == '<' || *linha == '>') {
            char *valor = extrair_ate(linha, 1);
            if (valor == NULL) {
                *erro = ERRO_MEMORIA;
                liberar_tokens(tokens, total);
                return NULL;
            }

            novoToken.tipo = TOKEN_SIMBOLO;
            novoToken.valor = valor;

            linha += 1;
        }

        else {
            int quantCaracteres = 0;
            while (linha[quantCaracteres] != '\n' &&
                linha[quantCaracteres] != '\0' &&
                linha[quantCaracteres] != '"'  &&
                linha[quantCaracteres] != '#'  &&
                !isspace((unsigned char)linha[quantCaracteres]) &&
                linha[quantCaracteres] != '$'  &&
                linha[quantCaracteres] != '<'  &&
                linha[quantCaracteres] != '>') {
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

Instrucao *montar_instrucoes(char **linhas, int quantidadeLinhas, int *quantidadeInstrucoes, ErroLexer *erro){
    int capacidade = 4;
    int total = 0;

    Instrucao *instrucoes = malloc(capacidade * sizeof(Instrucao));

    if (instrucoes == NULL){
        printf("ERRO: sem memória");
        return NULL;
    }

    for (int i = 0; i < quantidadeLinhas; i ++){
        if (total == capacidade){
            capacidade *= 2;
            Instrucao *temp = realloc(instrucoes, capacidade * sizeof(Instrucao));

            if (temp == NULL){
                *erro = ERRO_MEMORIA;
                for (int i = 0; i < total; i++){
                    liberar_tokens(instrucoes[i].tokens, instrucoes[i].quantidade);
                }
                free(instrucoes);
                return NULL;
            }
            instrucoes = temp;
        }

        Instrucao novaInstrucao;
        
        int quantidadeTokens, numLinha = i + 1;
        Token *tokens = tokenizar(linhas[i], &quantidadeTokens, erro);

        if (tokens == NULL){
            if (*erro == ERRO_ASPAS_NAO_FECHADAS) {
                printf("Erro de Sintaxe: linha %d: aspas não fechadas\n", numLinha);
            } else if (*erro == ERRO_MEMORIA) {
                printf("Erro: sem memória!\n");
            }

            for (int k = 0; k < total; k++){
                liberar_tokens(instrucoes[k].tokens, instrucoes[k].quantidade);
            }
            free(instrucoes);
            return NULL;
        }

        novaInstrucao.tokens = tokens;
        novaInstrucao.quantidade = quantidadeTokens;
        novaInstrucao.numLinha = numLinha;

        instrucoes[total] = novaInstrucao;
        total++;
    }

    *quantidadeInstrucoes = total;
    return instrucoes;
}

void liberar_instrucoes(Instrucao *instrucoes, int total){
    for (int i = 0; i < total; i++){
        liberar_tokens(instrucoes[i].tokens, instrucoes[i].quantidade);
    }
    free(instrucoes);
}