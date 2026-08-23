#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"

char *pular_espacos(char *linha) {
    while (*linha == ' ') {
        linha++;
    }
    return linha;
}


int executar_print(char *linha, EstadoExecucao *estado, int numLinha){

    if (*estado == PARADO){
        printf("Erro de Sintaxe: linha %d comando '<' fora de inicialização: '$ start'", numLinha);
        *estado = INTERROMPIDO;
        return 1;
    }

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
        printf("Erro de Sintaxe: linha %d comando '<' (aspas \"\" ausentes)", numLinha);
        *estado = INTERROMPIDO;
        return 1;
    }

    char *texto;
    int tamanhoTexto = posicoes[1] - posicoes[0];
    texto = malloc(tamanhoTexto * sizeof(char) + 1);

    if (texto == NULL) {
        printf("Erro: sem memória!\n");
        *estado = INTERROMPIDO;
        return 1;
    }

    int cont = 0;
    for (int i = posicoes[0] + 1; i < posicoes[1]; i++){
        texto[cont] = linha[i];
        cont++;
    }

    texto[tamanhoTexto - 1] = '\0';

    printf("%s\n", texto);

    free(texto);

    return 0;
}


int verificar_estado(char *linha, EstadoExecucao *estado){

    linha = pular_espacos(linha);  // agora linha já chega sem o '$', só pula espaço antes da palavra

    int tamanho = 0;
    while (linha[tamanho] != ' ' && linha[tamanho] != '\n' && linha[tamanho] != '\0'){
        tamanho++;
    }

    char *comando = malloc(tamanho * sizeof(char) + 1);
    if (comando == NULL) {
        printf("Erro: sem memória!\n");
        *estado = INTERROMPIDO;
        return 1;
    }

    for (int i = 0; i < tamanho; i++){
        comando[i] = linha[i];
    }
    comando[tamanho] = '\0';

    if (strcmp(comando, "start") == 0){
        *estado = RODANDO;
    }
    else if (strcmp(comando, "stop") == 0){
        *estado = PARADO;
    }

    free(comando);

    return 0;
}

int identificar_comando(char *linha, EstadoExecucao *estado, int numLinha){

    linha = pular_espacos(linha);

    char simbolo = *linha;

    if (simbolo == '$'){
        linha++;                     // pula o próprio símbolo '$'
        linha = pular_espacos(linha); // pula espaços depois do símbolo
        return verificar_estado(linha, estado);
    }
    else if (simbolo == '<'){
        linha++;
        linha = pular_espacos(linha);
        return executar_print(linha, estado, numLinha);
    }
    else if (simbolo == '>'){
        printf("Comando de controle (reservado)\n");
    }
    else if (simbolo != '\n'){
        printf("Erro de Sintaxe: linha: %d comando '%c' inválido", numLinha, simbolo);
        *estado = INTERROMPIDO;
    }
    
    return 0;
}

