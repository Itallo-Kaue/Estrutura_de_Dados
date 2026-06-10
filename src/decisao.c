#include <stdio.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h" 
#include "decisao.h"

// fucao decisao
const char* decidir_algoritmo(PerfilEntrada p) {
    if (p.tamanho < 30) return "insertion";
    
    if (p.quase_ordenada) return "insertion2_0";

    if (p.densidade_duplicatas > 60.0) return "merge";

    if (p.tamanho > 1000) return "quick"; 
    
    if (p.tamanho > 100) return "shell";

    return "heap"; 
}

// função retorna struct de métricas parte 4
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n) {
    MetricasExecucao m = {0}; // por seguranca inicializa zerado

    if (strcmp(nome, "insertion") == 0) {
        m = medir_insertionSort(arr, n);
    } else if (strcmp(nome, "insertion2_0") == 0) {
        m = medir_insertion2_0(arr, n);
    } else if (strcmp(nome, "quick") == 0) {
        m = medir_quickSort(arr, n);
    } else if (strcmp(nome, "merge") == 0) {
        m = medir_mergeSort(arr, n);
    } else if (strcmp(nome, "heap") == 0) {
        m = medir_heapSort(arr, n);
    } else if (strcmp(nome, "shell") == 0) {
        m = medir_shellSort(arr, n);
    }
    
    return m;
}