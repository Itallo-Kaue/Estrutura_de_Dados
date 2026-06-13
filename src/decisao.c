#include <stdio.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h" 
#include "decisao.h"

/* Seleciona o algoritmo mais adequado com base no perfil da entrada.
   Ordem de prioridade das regras: do caso mais específico ao mais geral. */
const char* decidir_algoritmo(PerfilEntrada p) {
    /* Counting Sort é vantajoso apenas quando a amplitude é pequena em termos
       absolutos E proporcionalmente ao tamanho, evitando desperdício de memória */
    if (p.amplitude > 0 && p.amplitude < 100000 && p.amplitude < (long long)(10 * p.tamanho)) {
        return "counting";
    }
    
     /* Insertion Sort é ideal para entradas pequenas ou quase ordenadas,
       pois tem baixo overhead e aproveita a ordem existente */
    if (p.tamanho < 50 || p.quase_ordenada) {
        return "insertion";
    }
    /* Merge Sort lida bem com muitos duplicatas por ser estável e não
       degradar com elementos iguais, ao contrário do Heap Sort */
    if (p.densidade_duplicatas > 50.0) {
        return "merge";
    }
    /* Heap Sort garante O(n log n) no pior caso com memória O(1),
       sendo a escolha segura para entradas grandes e desconhecidas */
    if (p.tamanho >= 1000) {
        return "heap";
    }

    /* Para entradas médias sem características especiais, Selection Sort
       minimiza o número de trocas, útil quando movimentação é custosa */
    return "selection";
    //ATENÇÃO ARRUMAR ESSA LÓGICA, PARECE ERRADA

}

// função retorna struct de métricas parte 4
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n) {
    MetricasExecucao m = {0}; // Inicialização defensiva

    if (strcmp(nome, "insertion") == 0) {
        m = medir_insertionSort(arr, n);
    } else if (strcmp(nome, "selection") == 0) {
        m = medir_selectionSort(arr, n)
    } else if (strcmp(nome, "quick") == 0) {
        m = medir_quickSort(arr, n);
    } else if (strcmp(nome, "merge") == 0) {
        m = medir_mergeSort(arr, n);
    } else if (strcmp(nome, "heap") == 0) {
        m = medir_heapSort(arr, n);
    } else if (strcmp(nome, "counting") == 0) {
        m = medir_countingSort(arr, n);
    }
    
    return m;
}