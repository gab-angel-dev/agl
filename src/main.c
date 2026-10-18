#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "executor.h"
#include "templates.h"
#include "util.h"

#define VERSAO "0.3.0"
#define MAX_LINHA 4096

static void usar(FILE *saida) {
    fprintf(saida,
        "AGL %s - automação de estrutura de projetos\n"
        "\n"
        "Uso:\n"
        "  agl run <arquivo.agl>        executa o arquivo a partir do diretório atual\n"
        "  agl run template <nome>      executa um template embutido\n"
        "  agl templates                lista os templates disponíveis\n"
        "  agl check <arquivo.agl>      só valida a sintaxe, sem executar\n"
        "  agl version                  mostra a versão\n"
        "  agl help                     mostra esta ajuda\n",
        VERSAO);
}

static void liberar_linhas(char **linhas, int total) {
    for (int i = 0; i < total; i++) {
        free(linhas[i]);
    }
    free(linhas);
}

// Acrescenta uma linha (já alocada) ao vetor. Assume a posse de 'linha'. 0 ok, 1 falha.
static int empilhar_linha(char ***linhas, int *total, int *capacidade, char *linha) {
    if (linha == NULL) {
        return 1;
    }

    if (*total == *capacidade) {
        int novaCapacidade = *capacidade * 2;
        char **temp = realloc(*linhas, (size_t)novaCapacidade * sizeof(char *));
        if (temp == NULL) {
            free(linha);
            return 1;
        }
        *linhas = temp;
        *capacidade = novaCapacidade;
    }

    (*linhas)[(*total)++] = linha;
    return 0;
}

static char **ler_arquivo(FILE *file, int *quantidadeLinhas) {
    int capacidade = 4;
    int total = 0;

    char **linhas = malloc((size_t)capacidade * sizeof(char *));
    if (linhas == NULL) {
        agl_erro(0, "sem memória");
        return NULL;
    }

    char buffer[MAX_LINHA];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {

        // linha maior que o buffer: o fgets a cortou no meio
        size_t len = strlen(buffer);
        if (len == sizeof(buffer) - 1 && buffer[len - 1] != '\n') {
            int c = fgetc(file);
            if (c != EOF) {
                ungetc(c, file);
                agl_erro(total + 1, "linha maior que %d caracteres", MAX_LINHA - 1);
                liberar_linhas(linhas, total);
                return NULL;
            }
        }

        if (empilhar_linha(&linhas, &total, &capacidade, copiar_string(buffer))) {
            agl_erro(0, "sem memória");
            liberar_linhas(linhas, total);
            return NULL;
        }
    }

    *quantidadeLinhas = total;
    return linhas;
}

// Mesma coisa que ler_arquivo, mas a partir de um texto em memória (templates).
static char **dividir_linhas(const char *texto, int *quantidadeLinhas) {
    int capacidade = 4;
    int total = 0;

    char **linhas = malloc((size_t)capacidade * sizeof(char *));
    if (linhas == NULL) {
        agl_erro(0, "sem memória");
        return NULL;
    }

    while (*texto != '\0') {
        const char *fim = strchr(texto, '\n');
        size_t len = fim ? (size_t)(fim - texto) + 1 : strlen(texto);   // inclui o '\n'

        char *copia = malloc(len + 1);
        if (copia != NULL) {
            memcpy(copia, texto, len);
            copia[len] = '\0';
        }

        if (empilhar_linha(&linhas, &total, &capacidade, copia)) {
            agl_erro(0, "sem memória");
            liberar_linhas(linhas, total);
            return NULL;
        }
        texto += len;
    }

    *quantidadeLinhas = total;
    return linhas;
}

// Tokeniza e (opcionalmente) executa. Libera 'linhas'.
static int processar_linhas(char **linhas, int quantLinhas, int apenas_checar) {
    int quantInstrucoes = 0;
    ErroLexer codigoErro = ERRO_NENHUM;
    Instrucao *instrucoes = montar_instrucoes(linhas, quantLinhas, &quantInstrucoes, &codigoErro);

    int status = 1;
    if (instrucoes != NULL) {
        if (apenas_checar) {
            printf("OK: %d linha(s), sintaxe válida\n", quantLinhas);
            status = 0;
        } else {
            status = executar(instrucoes, quantInstrucoes);
        }
        liberar_instrucoes(instrucoes, quantInstrucoes);
    }

    liberar_linhas(linhas, quantLinhas);
    return status;
}

static int processar_arquivo(const char *caminho, int apenas_checar) {
    FILE *file = fopen(caminho, "r");
    if (file == NULL) {
        return agl_erro(0, "não foi possível abrir o arquivo '%s'", caminho);
    }

    int quantLinhas = 0;
    char **linhas = ler_arquivo(file, &quantLinhas);
    fclose(file);

    if (linhas == NULL) {
        return 1;
    }
    return processar_linhas(linhas, quantLinhas, apenas_checar);
}

static int processar_template(const char *nome) {
    const Template *t = buscar_template(nome);
    if (t == NULL) {
        return agl_erro(0, "template '%s' não existe (veja: agl templates)", nome);
    }

    int quantLinhas = 0;
    char **linhas = dividir_linhas(t->conteudo, &quantLinhas);
    if (linhas == NULL) {
        return 1;
    }
    return processar_linhas(linhas, quantLinhas, 0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usar(stderr);
        return 1;
    }

    const char *sub = argv[1];

    if (strcmp(sub, "help") == 0 || strcmp(sub, "--help") == 0 || strcmp(sub, "-h") == 0) {
        usar(stdout);
        return 0;
    }

    if (strcmp(sub, "version") == 0 || strcmp(sub, "--version") == 0 || strcmp(sub, "-v") == 0) {
        printf("agl %s\n", VERSAO);
        return 0;
    }

    if (strcmp(sub, "templates") == 0) {
        listar_templates();
        return 0;
    }

    if (strcmp(sub, "run") == 0) {
        if (argc == 4 && strcmp(argv[2], "template") == 0) {
            return processar_template(argv[3]);
        }
        if (argc != 3 || strcmp(argv[2], "template") == 0) {
            fprintf(stderr, "Uso: agl run <arquivo.agl>  |  agl run template <nome>\n");
            return 1;
        }
        return processar_arquivo(argv[2], 0);
    }

    if (strcmp(sub, "check") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Uso: agl check <arquivo.agl>\n");
            return 1;
        }
        return processar_arquivo(argv[2], 1);
    }

    fprintf(stderr, "Subcomando desconhecido: '%s'\n\n", sub);
    usar(stderr);
    return 1;
}