#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#include "lexer.h"

int main(){
    int n;
    ErroLexer codigoErro = ERRO_NENHUM;
    Token *tokens = tokenizar("print \"Ola Mundo\n", &n, &codigoErro);
    for (int i = 0; i < n; i++) {
        printf("Token %d: tipo=%d valor=%s\n", i, tokens[i].tipo, tokens[i].valor);
    }
    return 0;
}