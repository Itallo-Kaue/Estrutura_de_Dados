#include <stdio.h>
#include <stdlib.h>
#include "metricas.h"
#include "execucao.h"
#include "decisao.h"

void imprimir_array(int *arr, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void resetar_dados(int *destino, int *origem, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        destino[i] = origem[i];
    }
}

const char* decidir_algoritmo(PerfilEntrada p);
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n);

int main() {
    int dados_originais[] = {10, 2, 5, 5, 2, 8, 9, 1, 100};
    int tamanho = sizeof(dados_originais) / sizeof(dados_originais[0]);

    // Extrair as métricas de perfil da entrada (Parte 2)
    PerfilEntrada perfil = analisar_input(dados_originais, tamanho);

    printf("--- Perfil da Entrada ---\n");
    printf("Tamanho: %d\n", perfil.tamanho);
    printf("Amplitude: %lld\n", perfil.amplitude);
    printf("Quantidade de Repetidos: %d\n", perfil.repetidos);
    printf("Desordem Estimada: %.2f%%\n", perfil.desordem_estimada_pct);
    printf("Tamanho Medio dos Runs: %.2f\n", perfil.tamanho_medio_runs);
    printf("Densidade de Duplicatas: %.2f%%\n", perfil.densidade_duplicatas);

    int dados_teste[tamanho];
    MetricasExecucao m;

    printf("\n--- Metricas de Execucao (Parte 4) ---\n\n");

    // --- TESTE 1: Insertion Sort Clássico ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m = medir_insertionSort(dados_teste, tamanho);
    imprimir_metricas("Insertion Sort Classico", m);

    // --- TESTE 2: Insertion Sort 2.0 ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m = medir_insertion2_0(dados_teste, tamanho);
    imprimir_metricas("Insertion Sort 2.0", m);

    // --- TESTE 3: Quick Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m = medir_quickSort(dados_teste, tamanho);
    imprimir_metricas("Quick Sort", m);

    // --- TESTE 4: Merge Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m = medir_mergeSort(dados_teste, tamanho);
    imprimir_metricas("Merge Sort", m);

    // --- TESTE 5: Heap Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m = medir_heapSort(dados_teste, tamanho);
    imprimir_metricas("Heap Sort", m);

    // --- TESTE 6: Shell Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    m = medir_shellSort(dados_teste, tamanho);
    imprimir_metricas("Shell Sort", m);

     printf("=== SISTEMA ADAPTATIVO: TESTE DE INTEGRACAO ===\n");

    // analise 
    PerfilEntrada perf = analisar_input(dados_originais, tamanho);

    // decisao
    const char* algoritmo_escolhido = decidir_algoritmo(perf);
    printf("Algoritmo selecionado pela heuristica: %s\n", algoritmo_escolhido);

    // execucao e coleta de dados
    MetricasExecucao m = executar_com_inteligencia(algoritmo_escolhido, dados_originais, Tamanho);

    // resultado
    printf("\nResultado da Execucao:\n");
    imprimir_metricas(algoritmo_escolhido, m);

    return 0;
}