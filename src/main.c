#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "executor.h"

char **ler_arquivo(FILE *file, int *quantidadeLinhas) {
    int capacidade = 4;              // quantas linhas cabem hoje no array
    int total = 0;                   // quantas linhas já lemos de fato

    char **linhas = malloc(capacidade * sizeof(char *));
    if (linhas == NULL) {
        printf("Erro: sem memória!\n");
        return NULL;
    }

    char buffer[1000];

    while (fgets(buffer, 1000, file) != NULL) {

        if (total == capacidade) {
            capacidade = capacidade * 2;
            char **temp = realloc(linhas, capacidade * sizeof(char *));

            if (temp == NULL) {
                printf("Erro: sem memória!\n");
                // libera o que já tinha sido copiado até agora
                for (int i = 0; i < total; i++) {
                    free(linhas[i]);
                }
                free(linhas);
                return NULL;
            }

            linhas = temp;
        }

        linhas[total] = strdup(buffer);
        if (linhas[total] == NULL) {
            printf("Erro: sem memória!\n");
            for (int i = 0; i < total; i++) {
                free(linhas[i]);
            }
            free(linhas);
            return NULL;
        }

        total++;
    }

    *quantidadeLinhas = total;
    return linhas;
}

int main(int argc, char *argv[]){

    if (argc != 2){
        printf("É preciso passar o arquivo -> Uso: ./agl arquivo.agl\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL){
        printf("Erro: não foi possível abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    int quantLinhas = 0;
    char **linhas = ler_arquivo(file, &quantLinhas);

    fclose(file);

    if (linhas == NULL) {
        return 1;
    }

    // TOKENIZAR
    int quantInstrucoes = 0;
    ErroLexer codigoErro = ERRO_NENHUM;
    Instrucao *instrucoes = montar_instrucoes(linhas, quantLinhas, &quantInstrucoes, &codigoErro);

    if (instrucoes == NULL) {
        for (int i = 0; i < quantLinhas; i++) {
            free(linhas[i]);
        }
        free(linhas);
        return 1;
    }

    // EXECUTAR
    int status = executar(instrucoes, quantInstrucoes);

    // LIBERAR
    liberar_instrucoes(instrucoes, quantInstrucoes);

    for (int i = 0; i < quantLinhas; i++){
        free(linhas[i]);
    }
    free(linhas);

    return status;
}