#include <stdio.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h" 
#include "decisao.h"

// fucao decisao
const char* decidir_algoritmo(PerfilEntrada p) {
    // p range pequeno, independente do tam do vetor
    if (p.amplitude > 0 && p.amplitude < (1.5 * p.tamanho)) {
        return "counting";
    }
    
    // 2. p vetores pequenos / quase ordenados
    if (p.tamanho < 64 || p.quase_ordenada) {
        return "insertion";
    }
    
    // quando tem muitas duplicatas e estabilidade importa
    if (p.densidade_duplicatas > 60.0) {
        return "merge";
    }
    
    // para grandes volumes
    if (p.tamanho >= 1000) {
        return "heap";
    } 
    
    // Fallback p casos medios
    return "selection";
}

MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n) {
    if (strcmp(nome, "insertion") == 0) return medir_insertionSort(arr, n);
    if (strcmp(nome, "selection") == 0) return medir_selectionSort(arr, n);
    if (strcmp(nome, "heap") == 0)      return medir_heapSort(arr, n); // AJUSTADO
    if (strcmp(nome, "merge") == 0)     return medir_mergeSort(arr, n);
    if (strcmp(nome, "counting") == 0)  return medir_countingSort(arr, n);
    
    MetricasExecucao m = {0};
    return m;
}