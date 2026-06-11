#include <stdio.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h" 
#include "decisao.h"

// fucao decisao
const char* decidir_algoritmo(PerfilEntrada p) {
    if (p.amplitude > 0 && p.amplitude < (2 * p.tamanho)) {
        return "counting";
    }
    if (p.tamanho < 50 || p.quase_ordenada) {
        return "insertion";
    }
    if (p.densidade_duplicatas > 50.0) {
        return "merge";
    }
    if (p.tamanho >= 1000) {
        return "quick";
    } 
    return "selection";
}

// função retorna struct de métricas parte 4
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n) {
    MetricasExecucao m = {0}; // Inicialização defensiva

    if (strcmp(nome, "insertion") == 0) {
        m = medir_insertionSort(arr, n);
    } else if (strcmp(nome, "selection") == 0) {
        m = medir_selectionSort(arr, n);
    } else if (strcmp(nome, "quick") == 0) {
        m = medir_quickSort(arr, n);
    } else if (strcmp(nome, "merge") == 0) {
        m = medir_mergeSort(arr, n);
    } else if (strcmp(nome, "counting") == 0) {
        m = medir_countingSort(arr, n);
    }
    
    return m;
}