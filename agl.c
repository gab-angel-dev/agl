#include <stdio.h>


int main(int argc, char *argv[]){

    if (argc != 2){
        printf("É preciso passar o arquivo -> Uso: ./agl arquivo.agl\n");
        return 1;
    }
    else {
        FILE *file = fopen(argv[1], "r");

        if (file == NULL){
            printf("Erro: não foi possível abrir o arquivo '%s'\n", argv[1]);
            return 1;
        }

        printf("Arquivo aberto com sucesso!\n");
        
        char linha[1000];

        while (fgets(linha, 1000, file) != NULL){
            printf("%s", linha);
        }
        printf("\n");

        fclose(file);
    }
    

    return 0;
}