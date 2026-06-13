#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h"
#include "decisao.h"

void imprimir_array(int *arr, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int* gerar_dados_aleatorios(int tamanho) {
    int *arr = malloc(tamanho * sizeof(int));
    if (!arr) return NULL;
    for (int i = 0; i < tamanho; i++) {
        arr[i] = rand() % 100000; // Gera números de 0 a 99999
    }
    return arr;
}

void resetar_dados(int *destino, int *origem, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        destino[i] = origem[i];
    }
}

int* ler_arquivo(const char *caminho, int *tamanho_saida) {
    FILE *file = fopen(caminho, "r");
    if (!file) {
        fprintf(stderr, "Erro de Execucao: Nao foi possivel abrir o arquivo '%s'\n", caminho);
        return NULL;
    }

    int capacidade = 2000;
    int *arr = malloc(capacidade * sizeof(int));
    int contador = 0;

    if (!arr) {
        fclose(file);
        return NULL;
    }

    int valor;
    // Lê inteiros sequencialmente até atingir o fim do arquivo (EOF)
    while (fscanf(file, "%d", &valor) == 1) {
        if (contador >= capacidade) {
            capacidade *= 2;
            int *temp = realloc(arr, capacidade * sizeof(int));
            if (!temp) {
                fprintf(stderr, "Erro Crítico: Falha de memoria ao realocar vetor.\n");
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

/*const char* decidir_algoritmo(PerfilEntrada p);
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n); */

int main(int argc, char *argv[]) {
    srand(time(NULL));
    // 1. Criar uma lógica para identificar os argumentos:
    // Exemplo simplificado de busca de parâmetros:
    char *caminho_input = NULL;
    char *modo = "adaptativo"; // Modo padrão solicitado pelo PDF
    char *algoritmo_fixo = "quick"; // Algoritmo fixo padrão
    /*int tamanho_gerar = 0;*/

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--modo") == 0 && i + 1 < argc) {
            modo = argv[i + 1];
        }
        if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            caminho_input = argv[i + 1];
        }
        if (strcmp(argv[i], "--algoritmo") == 0 && i + 1 < argc) {
            algoritmo_fixo = argv[i + 1];
        }
    }

    // 2. Se um arquivo foi passado, carregar os dados dele dinamicamente
    // Se não, exibir uma mensagem de instrução de uso.
    if (caminho_input == NULL) {
        printf("Uso: %s --modo <adaptativo|fixo> --input <arquivo.txt>\n", argv[0]);
        return 1;
    }
    int *dados_originais = NULL;
    int tamanho = 0;

    /* Definição da fonte de dados de entrada
    if (caminho_input != NULL) {
        dados_originais = ler_arquivo(caminho_input, &tamanho);
        if (!dados_originais) return 1;
    } else if (tamanho_gerar > 0) {
        tamanho = tamanho_gerar;
        dados_originais = gerar_dados_aleatorios(tamanho);
        if (!dados_originais) return 1;
    } else {
        // Fallback defensivo com dados estáticos se nenhum parâmetro for informado
        printf("[Aviso] Nenhum parametro fornecido. Usando vetor de teste padrao.\n");
        int arr_padrao[] = {10, 2, 5, 5, 2, 8, 9, 1, 100};
        tamanho = sizeof(arr_padrao) / sizeof(arr_padrao[0]);
        dados_originais = malloc(tamanho * sizeof(int));
        resetar_dados(dados_originais, arr_padrao, tamanho);*/
    dados_originais = ler_arquivo(caminho_input, &tamanho);
    if (!dados_originais || tamanho == 0) {
    fprintf(stderr, "Erro: Vetor de dados vazio ou falha na leitura do arquivo.\n");
    return 1;
    }

    // Extrair as métricas de perfil da entrada (Parte 2)
    PerfilEntrada perfil = analisar_input(dados_originais, tamanho);

    printf("===========================================\n");
    printf("         RELATORIO DE PERFIL DA ENTRADA    \n");
    printf("===========================================\n");
    printf("Tamanho do Vetor         : %d\n", perfil.tamanho);
    printf("Amplitude dos Valores    : %lld\n", perfil.amplitude);
    printf("Quantidade de Repetidos  : %d\n", perfil.repetidos);
    printf("Desordem Estimada        : %.2f%%\n", perfil.desordem_estimada_pct);
    printf("Quase Ordenada           : %s\n", perfil.quase_ordenada ? "Sim" : "Nao");
    printf("Padroes Sequenciais      : %d\n", perfil.padroes_sequenciais);
    printf("Tamanho Medio dos Runs   : %.2f\n", perfil.tamanho_medio_runs);
    printf("Densidade de Duplicatas  : %.2f%%\n", perfil.densidade_duplicatas);
    printf("Media dos Valores        : %.2f\n", perfil.media_valores);
    printf("Tipo de Distribuicao     : %s\n",
           perfil.tipo_distribuicao < 0 ? "Concentrada nos menores" :
           perfil.tipo_distribuicao > 0 ? "Concentrada nos maiores" : "Uniforme/equilibrada");
    printf("-------------------------------------------\n\n");
    
    // Aloca vetor de trabalho dinâmico para a execução
    int *dados_teste = malloc(tamanho * sizeof(int));
    if (!dados_teste) {
        free(dados_originais);
        return 1;
    }
    resetar_dados(dados_teste, dados_originais, tamanho);

    MetricasExecucao m_resultado;
    const char *algoritmo_utilizado = NULL;

    // Execução controlada pelo modo selecionado
    if (strcmp(modo, "adaptativo") == 0) {
        printf(">>> MODO ADAPTATIVO ATIVADO <<<\n");
        algoritmo_utilizado = decidir_algoritmo(perfil);
        printf("Algoritmo selecionado pela heuristica: %s\n\n", algoritmo_utilizado);
        m_resultado = executar_com_inteligencia(algoritmo_utilizado, dados_teste, tamanho);
    } else {
        printf(">>> MODO FIXO ATIVADO <<<\n");
        printf("Algoritmo determinado manualmente: %s\n\n", algoritmo_fixo);
        algoritmo_utilizado = algoritmo_fixo;
        resetar_dados(dados_teste, dados_originais, tamanho);
        m_resultado = executar_com_inteligencia(algoritmo_fixo, dados_teste, tamanho);
    }

    // Exibição formatada dos resultados coletados
    printf("===========================================\n");
    printf("           METRICAS DE EXECUCAO final      \n");
    printf("===========================================\n");
    imprimir_metricas(algoritmo_utilizado, m_resultado);
    printf("===========================================\n");

    printf("\n--- Metricas de Execucao (Parte 4) ---\n\n");

// --- TESTE 1: Insertion Sort Clássico ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m_resultado = medir_insertionSort(dados_teste, tamanho);
    imprimir_metricas("Insertion Sort Classico", m_resultado);

    // --- TESTE 2: Selection Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m_resultado = medir_selectionSort(dados_teste, tamanho);
    imprimir_metricas("Selection Sort", m_resultado);

    // --- TESTE 3: Heap Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m_resultado = medir_heapSort(dados_teste, tamanho);
    imprimir_metricas("Heap Sort", m_resultado);

    // --- TESTE 4: Merge Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m_resultado = medir_mergeSort(dados_teste, tamanho);
    imprimir_metricas("Merge Sort", m_resultado);

    // --- TESTE 5: Counting Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m_resultado = medir_countingSort(dados_teste, tamanho);
    imprimir_metricas("Counting Sort", m_resultado);

    // Liberação de memória alocada dinamicamente
    free(dados_teste);
    free(dados_originais);

    return 0;
}