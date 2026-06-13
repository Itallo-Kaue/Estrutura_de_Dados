#include <string.h>
#include "metricas.h"
#include "execucao.h"
#include "decisao.h"

// Seleciona o algoritmo mais adequado com base no perfil da entrada.
// Ordem de prioridade das regras: do caso mais específico ao mais geral

const char *decidir_algoritmo(PerfilEntrada p) {
    // Counting Sort: vantajoso apenas quando amplitude é pequena em termos absolutos e proporcionalmente ao tamanho (evita desperdício de memória)
    if (p.amplitude > 0 && p.amplitude < 100000 && p.amplitude < (long long)(10 * p.tamanho))
        return "counting";

    // Insertion Sort: ideal para entradas pequenas ou quase ordenadas (possui um baixo overhead e aproveita ordem existente)
    if (p.tamanho < 50 || p.quase_ordenada)
        return "insertion";

    // Merge Sort: vantajoso com muito duplicatas por ser estável e não degradar com elem.iguais (contrário do Heap Sort)
    if (p.densidade_duplicatas > 50.0)
        return "merge";

    // Heap Sort: garante O(n log n) no pior caso com memória O(1), sendo a escolha segura para entradas grandes e desconhecidas
    if (p.tamanho >= 1000)
        return "heap";

    // Selection Sort: para entradas médias sem características especiais minimiza o número de trocas, útil quando movimentação é custosa
    return "selection";
}

MetricasExecucao executar_algoritmo(const char *nome, int *arr, int n) {
    MetricasExecucao m = {0};

    if      (strcmp(nome, "insertion") == 0) m = medir_insertionSort(arr, n);
    else if (strcmp(nome, "selection") == 0) m = medir_selectionSort(arr, n);
    else if (strcmp(nome, "heap")      == 0) m = medir_heapSort(arr, n);
    else if (strcmp(nome, "merge")     == 0) m = medir_mergeSort(arr, n);
    else if (strcmp(nome, "counting")  == 0) m = medir_countingSort(arr, n);

    return m;
}
