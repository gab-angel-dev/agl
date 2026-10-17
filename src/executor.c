#include <stdio.h>
#include <string.h>
#include "executor.h"
#include "comandos.h"

#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

typedef struct {
    const char *nome;
    int (*funcao)(const Instrucao *, Contexto *);
} Comando;

static const Comando tabela[] = {
    {"create", cmd_create},
    {"print",  cmd_print},
    {"bash",   cmd_bash},
};

static const Comando *buscar_comando(const char *nome) {
    size_t tamanho = sizeof(tabela) / sizeof(tabela[0]);

    for (size_t i = 0; i < tamanho; i++){
        if (strcmp(nome, tabela[i].nome) == 0){
            return &tabela[i];
        }
    }

    return NULL;
}

static void tratar_estado(const Instrucao *ins, Contexto *ctx){
    if (&ins->tokens[1].valor[0] == "start"){
        ctx->estado = RODANDO;
        
        getcwd(ctx->diretorioBase, sizeof(ctx->diretorioBase));
    
        if (ctx->diretorioBase == NULL){
            printf("Erro ao obter diretório");
        }
    }
    else if (&ins->tokens[1].valor[0] == "stop")
    {
        ctx->estado = PARADO;

    }
}

int executar(Instrucao *instrucoes, int total) {
    Contexto ctx = { PARADO, "." };

    for (int i = 0; i < total && ctx.estado != INTERROMPIDO; i++) {
        // TODO: pular instrução vazia
        // TODO: buscar o comando pelo tokens[0]
        // TODO: chamar a função
        const Instrucao *ins = &instrucoes[i];
        
        if (ins->quantidade == 0) continue;

        if (ins->tokens[0].tipo == TOKEN_SIMBOLO){
            if (ins->tokens[0].valor[0] == '$'){
                tratar_estado(ins, &ctx);
            }
            continue;   
        }
        
        if (ins->tokens[0].tipo != TOKEN_PALAVRA){
            printf("Linha %d: comando inválido\n", ins->numLinha);
            ctx.estado = INTERROMPIDO;
            break;
        }

        const Comando *cmd = buscar_comando(ins->tokens[0].valor);

        if (cmd == NULL){
            printf("Linha %d: comando '%s' desconhecido\n", ins->numLinha, ins->tokens[0].valor);
            ctx.estado = INTERROMPIDO;
        }
        else
        {
            int result = cmd->funcao(ins, &ctx);

            if (result != 0){
                ctx.estado = INTERROMPIDO;
            }
        }
    }

    return ctx.estado == INTERROMPIDO ? 1 : 0;
}