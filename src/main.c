#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#include "lexer.h"

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

    fclose(file); // já podemos fechar o arquivo — todo o conteúdo já está em memória

    if (linhas == NULL) {
        return 1; // ler_arquivo já imprimiu o erro
    }

    for (int i = 0; i < quantLinhas; i++){
        printf("%s", linhas[i]);
    }
    printf("\n%d\n", quantLinhas);
    // teste

    // próximo passo: tokenizar cada linha aqui

    for (int i = 0; i < quantLinhas; i++){
        free(linhas[i]);
    }

    

    free(linhas);

    return 0;
}