#include <stdio.h>
#include <stdlib.h>
#include "metricas.h" 

// Função auxiliar para imprimir o array na tela
void imprimir_array(int *arr, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
// Função auxiliar para copiar os dados originais e resetar o array de teste
void resetar_dados(int *destino, int *origem, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        destino[i] = origem[i];
    }
}

int main() {
    int dados_originais[] = {10, 2, 5, 5, 2, 8, 9, 1, 100};
    int tamanho = sizeof(dados_originais) / sizeof(dados_originais[0]);
    
    // Extrair as métricas
    PerfilEntrada perfil = analisar_input(dados_originais, tamanho);
    
    printf("--- Perfil da Entrada Otimizado ---\n");
    printf("Tamanho: %d\n", perfil.tamanho);
    printf("Amplitude: %lld\n", perfil.amplitude);
    printf("Quantidade de Repetidos: %d\n", perfil.repetidos);
    printf("Desordem Estimada: %.2f%%\n", perfil.desordem_estimada_pct);
    printf("Tamanho Medio dos Runs: %.2f\n", perfil.tamanho_medio_runs);
    printf("Densidade de Duplicatas: %.2f%%\n", perfil.densidade_duplicatas);
    
    int dados_teste[tamanho];
    // --- TESTE 1: Insertion Sort Clássico ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    printf("\n[Insertion Sort Clássico]\nOriginal: ");
    imprimir_array(dados_teste, tamanho);
    insertionSort(dados_teste, tamanho);
    printf("Ordenado: ");
    imprimir_array(dados_teste, tamanho);

    // --- TESTE 2: Insertion Sort 2.0 (Slides) ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    printf("\n[Insertion Sort 2.0]\nOriginal: ");
    imprimir_array(dados_teste, tamanho);
    insertion2_0(dados_teste, tamanho);
    printf("Ordenado: ");
    imprimir_array(dados_teste, tamanho);

    // --- TESTE 3: Quick Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    printf("\n[Quick Sort]\nOriginal: ");
    imprimir_array(dados_teste, tamanho);
    quickSort(dados_teste, 0, tamanho - 1);
    printf("Ordenado: ");
    imprimir_array(dados_teste, tamanho);

    // --- TESTE 4: Merge Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    printf("\n[Merge Sort]\nOriginal: ");
    imprimir_array(dados_teste, tamanho);
    mergeSort(dados_teste, 0, tamanho - 1);
    printf("Ordenado: ");
    imprimir_array(dados_teste, tamanho);

    // --- TESTE 5: Heap Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    printf("\n[Heap Sort]\nOriginal: ");
    imprimir_array(dados_teste, tamanho);
    heapSort(dados_teste, tamanho);
    printf("Ordenado: ");
    imprimir_array(dados_teste, tamanho);

    // --- TESTE 6: Shell Sort ---
    resetar_dados(dados_teste, dados_originais, tamanho);
    printf("\n[Shell Sort]\nOriginal: ");
    imprimir_array(dados_teste, tamanho);
    shellSort(dados_teste, tamanho);
    printf("Ordenado: ");
    imprimir_array(dados_teste, tamanho);

    return 0;
}