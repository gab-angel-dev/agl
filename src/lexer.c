#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "util.h"

static char *extrair_ate(const char *inicio, int tamanho) {
    char *valor = malloc((size_t)tamanho + 1);
    if (valor == NULL) {
        return NULL;
    }
    memcpy(valor, inicio, (size_t)tamanho);
    valor[tamanho] = '\0';
    return valor;
}

static void liberar_tokens(Token *tokens, int total) {
    for (int i = 0; i < total; i++) {
        free(tokens[i].valor);
    }
    free(tokens);
}

// Pula espaços/tabs/\r, mas NÃO o '\n' (ele marca o fim da linha).
static char *pular_espacos(char *linha) {
    while (*linha != '\n' && isspace((unsigned char)*linha)) {
        linha++;
    }
    return linha;
}

// Acrescenta um token. Assume a posse de 'valor' (libera se falhar). 0 ok, 1 falha.
static int adicionar(Token **tokens, int *total, int *capacidade, TipoToken tipo, char *valor) {
    if (valor == NULL) {
        return 1;
    }

    if (*total == *capacidade) {
        int novaCapacidade = *capacidade * 2;
        Token *temp = realloc(*tokens, (size_t)novaCapacidade * sizeof(Token));
        if (temp == NULL) {
            free(valor);
            return 1;
        }
        *tokens = temp;
        *capacidade = novaCapacidade;
    }

    (*tokens)[*total].tipo = tipo;
    (*tokens)[*total].valor = valor;
    (*total)++;
    return 0;
}

static int fim_de_palavra(char c) {
    return c == '\0' || isspace((unsigned char)c) ||
           c == '"' || c == '#' || c == '$' || c == ',' || c == '>';
}

Token *tokenizar(char *linha, int *quantidadeTokens, ErroLexer *erro) {
    *erro = ERRO_NENHUM;
    *quantidadeTokens = 0;

    int capacidade = 4;
    int total = 0;

    Token *tokens = malloc((size_t)capacidade * sizeof(Token));
    if (tokens == NULL) {
        *erro = ERRO_MEMORIA;
        return NULL;
    }

    while (*linha != '\0' && *linha != '\n') {

        linha = pular_espacos(linha);

        if (*linha == '\0' || *linha == '\n') {
            break;
        }

        // comentário: ignora o resto da linha
        if (*linha == '#') {
            break;
        }

        // "string"
        if (*linha == '"') {
            linha++;
            int n = 0;
            while (linha[n] != '"') {
                if (linha[n] == '\0' || linha[n] == '\n') {
                    *erro = ERRO_ASPAS_NAO_FECHADAS;
                    liberar_tokens(tokens, total);
                    return NULL;
                }
                n++;
            }

            if (adicionar(&tokens, &total, &capacidade, TOKEN_STRING, extrair_ate(linha, n))) {
                goto erro_memoria;
            }
            linha += n + 1;
        }

        // símbolos: $  ,  >
        else if (*linha == '$' || *linha == ',' || *linha == '>') {
            char simbolo = *linha;

            if (adicionar(&tokens, &total, &capacidade, TOKEN_SIMBOLO, extrair_ate(linha, 1))) {
                goto erro_memoria;
            }
            linha++;

            // '>' : o resto da linha é um comando de shell, guardado sem interpretar
            if (simbolo == '>') {
                linha = pular_espacos(linha);

                int n = 0;
                while (linha[n] != '\0' && linha[n] != '\n') {
                    n++;
                }
                while (n > 0 && isspace((unsigned char)linha[n - 1])) {
                    n--;
                }

                if (adicionar(&tokens, &total, &capacidade, TOKEN_BRUTO, extrair_ate(linha, n))) {
                    goto erro_memoria;
                }

                while (*linha != '\0' && *linha != '\n') {
                    linha++;
                }
            }
        }

        // palavra
        else {
            int n = 0;
            while (!fim_de_palavra(linha[n])) {
                n++;
            }

            if (adicionar(&tokens, &total, &capacidade, TOKEN_PALAVRA, extrair_ate(linha, n))) {
                goto erro_memoria;
            }
            linha += n;
        }
    }

    *quantidadeTokens = total;
    return tokens;

erro_memoria:
    *erro = ERRO_MEMORIA;
    liberar_tokens(tokens, total);
    return NULL;
}

Instrucao *montar_instrucoes(char **linhas, int quantidadeLinhas, int *quantidadeInstrucoes, ErroLexer *erro) {
    *quantidadeInstrucoes = 0;
    *erro = ERRO_NENHUM;

    int capacidade = 4;
    int total = 0;

    Instrucao *instrucoes = malloc((size_t)capacidade * sizeof(Instrucao));
    if (instrucoes == NULL) {
        *erro = ERRO_MEMORIA;
        agl_erro(0, "sem memória");
        return NULL;
    }

    for (int i = 0; i < quantidadeLinhas; i++) {

        if (total == capacidade) {
            capacidade *= 2;
            Instrucao *temp = realloc(instrucoes, (size_t)capacidade * sizeof(Instrucao));

            if (temp == NULL) {
                *erro = ERRO_MEMORIA;
                agl_erro(0, "sem memória");
                liberar_instrucoes(instrucoes, total);
                return NULL;
            }
            instrucoes = temp;
        }

        int quantidadeTokens = 0;
        int numLinha = i + 1;
        Token *tokens = tokenizar(linhas[i], &quantidadeTokens, erro);

        if (tokens == NULL) {
            if (*erro == ERRO_ASPAS_NAO_FECHADAS) {
                agl_erro(numLinha, "aspas não fechadas");
            } else {
                agl_erro(numLinha, "sem memória");
            }
            liberar_instrucoes(instrucoes, total);
            return NULL;
        }

        instrucoes[total].tokens = tokens;
        instrucoes[total].quantidade = quantidadeTokens;
        instrucoes[total].numLinha = numLinha;
        total++;
    }

    *quantidadeInstrucoes = total;
    return instrucoes;
}

void liberar_instrucoes(Instrucao *instrucoes, int total) {
    for (int i = 0; i < total; i++) {
        liberar_tokens(instrucoes[i].tokens, instrucoes[i].quantidade);
    }
    free(instrucoes);
}