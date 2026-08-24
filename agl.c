#include <stdio.h>
#include "comandos.h"


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
        

    char linha[1000];

    EstadoExecucao estado = PARADO;
    int numLinha = 1;
    
    while (fgets(linha, 1000, file) != NULL && estado != INTERROMPIDO){
        identificar_comando(linha, &estado, numLinha);
        numLinha++;
    }
    printf("\n");

    fclose(file);
    


    return 0;
}