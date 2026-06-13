#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h"
#include "decisao.h"

#define CAPACIDADE_INICIAL_LEITURA 4096

static void resetar_dados(int *destino, const int *origem, int tamanho) {
    for (int i = 0; i < tamanho; i++)
        destino[i] = origem[i];
}

static int* ler_arquivo(const char *caminho, int *tamanho_saida) {
    FILE *file = fopen(caminho, "r");
    if (!file) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", caminho);
        return NULL;
    }

    int capacidade = CAPACIDADE_INICIAL_LEITURA;
    int *arr = malloc(capacidade * sizeof(int));
    int contador = 0;

    if (!arr) {
        fclose(file);
        return NULL;
    }

    int valor;
    while (fscanf(file, "%d", &valor) == 1) {
        if (contador >= capacidade) {
            capacidade *= 2;
            int *temp = realloc(arr, capacidade * sizeof(int));
            if (!temp) {
                fprintf(stderr, "Erro critico: falha de memoria ao realocar vetor.\n");
                free(arr);
                fclose(file);
                return NULL;
            }
            arr = temp;
        }
        arr[contador++] = valor;
    }

    fclose(file);
    *tamanho_saida = contador;
    return arr;
}

static void imprimir_perfil(PerfilEntrada perfil) {
    printf("===========================================\n");
    printf("         RELATORIO DE PERFIL DA ENTRADA    \n");
    printf("===========================================\n");
    printf("Tamanho do Vetor         : %d\n",    perfil.tamanho);
    printf("Amplitude dos Valores    : %lld\n",  perfil.amplitude);
    printf("Quantidade de Repetidos  : %d\n",    perfil.repetidos);
    printf("Desordem Estimada        : %.2f%%\n", perfil.desordem_estimada_pct);
    printf("Quase Ordenada           : %s\n",    perfil.quase_ordenada ? "Sim" : "Nao");
    printf("Padroes Sequenciais      : %d\n",    perfil.padroes_sequenciais);
    printf("Tamanho Medio dos Runs   : %.2f\n",  perfil.tamanho_medio_runs);
    printf("Densidade de Duplicatas  : %.2f%%\n", perfil.densidade_duplicatas);
    printf("Media dos Valores        : %.2f\n",  perfil.media_valores);
    printf("Tipo de Distribuicao     : %s\n",
           perfil.tipo_distribuicao < 0 ? "Concentrada nos menores" :
           perfil.tipo_distribuicao > 0 ? "Concentrada nos maiores" : "Uniforme/equilibrada");
    printf("-------------------------------------------\n\n");
}

/* Parte 4 — mede todos os algoritmos no mesmo dataset para comparação direta */
static void executar_comparacao_parte4(const int *dados_originais, int *dados_teste, int tamanho) {
    static const char *algoritmos[] = {
        "insertion", "selection", "heap", "merge", "counting"
    };
    static const char *nomes[] = {
        "Insertion Sort", "Selection Sort", "Heap Sort", "Merge Sort", "Counting Sort"
    };
    int n_algoritmos = (int)(sizeof(algoritmos) / sizeof(algoritmos[0]));

    printf("\n--- Metricas de Execucao (Parte 4) ---\n\n");

    for (int i = 0; i < n_algoritmos; i++) {
        resetar_dados(dados_teste, dados_originais, tamanho);
        MetricasExecucao m = executar_algoritmo(algoritmos[i], dados_teste, tamanho);
        imprimir_metricas(nomes[i], m);
    }
}

int main(int argc, char *argv[]) {
    const char *caminho_input  = NULL;
    const char *modo           = "adaptativo";
    const char *algoritmo_fixo = "heap";

    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "--modo")      == 0 && i + 1 < argc) modo           = argv[++i];
        else if (strcmp(argv[i], "--input")     == 0 && i + 1 < argc) caminho_input  = argv[++i];
        else if (strcmp(argv[i], "--algoritmo") == 0 && i + 1 < argc) algoritmo_fixo = argv[++i];
    }

    if (!caminho_input) {
        printf("Uso: %s --modo <adaptativo|fixo> --input <arquivo.txt> [--algoritmo <nome>]\n",
               argv[0]);
        return 1;
    }

    int tamanho = 0;
    int *dados_originais = ler_arquivo(caminho_input, &tamanho);
    if (!dados_originais || tamanho == 0) {
        fprintf(stderr, "Erro: vetor vazio ou falha na leitura do arquivo.\n");
        return 1;
    }

    PerfilEntrada perfil = analisar_input(dados_originais, tamanho);
    imprimir_perfil(perfil);

    int *dados_teste = malloc(tamanho * sizeof(int));
    if (!dados_teste) {
        free(dados_originais);
        return 1;
    }
    resetar_dados(dados_teste, dados_originais, tamanho);

    const char       *algoritmo_utilizado;
    MetricasExecucao  resultado;

    if (strcmp(modo, "adaptativo") == 0) {
        printf(">>> MODO ADAPTATIVO ATIVADO <<<\n");
        algoritmo_utilizado = decidir_algoritmo(perfil);
        printf("Algoritmo selecionado pela heuristica: %s\n\n", algoritmo_utilizado);
        resultado = executar_algoritmo(algoritmo_utilizado, dados_teste, tamanho);
    } else {
        printf(">>> MODO FIXO ATIVADO <<<\n");
        printf("Algoritmo determinado manualmente: %s\n\n", algoritmo_fixo);
        algoritmo_utilizado = algoritmo_fixo;
        resultado = executar_algoritmo(algoritmo_fixo, dados_teste, tamanho);
    }

    printf("===========================================\n");
    printf("           METRICAS DE EXECUCAO            \n");
    printf("===========================================\n");
    imprimir_metricas(algoritmo_utilizado, resultado);
    printf("===========================================\n");

    executar_comparacao_parte4(dados_originais, dados_teste, tamanho);

    free(dados_teste);
    free(dados_originais);
    return 0;
}
