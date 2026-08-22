#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int identificar_comando(char *linha){

    while (*linha == ' ' && *linha != '\0'){
        linha++;
    }

    char simbolo = *linha;

    if (simbolo == '$'){
        printf("Comando de sistema\n");
    }




    else if (simbolo == '<'){
        printf("Comando de saída\n");

        int posicoes[2];
        int tamanho = 0;
        int i = 0;

        while (linha[i] != '\n' && tamanho < 2){
            if (linha[i] == '"'){
                posicoes[tamanho] = i;
                tamanho++;
            }

            i++;
        }

        if (tamanho != 2){
            printf("Erro: sintaxe inválida no comando print (aspas ausentes)\n");
            return 1;
        }

        char *texto;
        int tamanhoTexto = posicoes[1] - posicoes[0];
        texto = malloc(tamanhoTexto * sizeof(char));

        if (texto == NULL) {
            printf("Erro: sem memória!\n");
            return 1;
        }

        int cont = 0;
        for (int i = posicoes[0] + 1; i < posicoes[1]; i++){
            texto[cont] = linha[i];
            cont++;
        }

        texto[tamanhoTexto - 1] = '\0';

        printf("%s", texto);

        free(texto);

    }







    else if (simbolo == '>'){
        printf("Comando de controle (reservado)\n");
    }
    else {
        printf("Comando desconhecido\n");
    }
    
    return 0;
}

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
            identificar_comando(linha);
        }
        printf("\n");

        fclose(file);
    }


    return 0;
}