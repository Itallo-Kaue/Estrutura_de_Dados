#include <stdio.h>
#include <string.h>
#include "metricas.h"
#include "execucao.h" 
#include "decisao.h"

/* Seleciona o algoritmo mais adequado com base no perfil da entrada.
   Ordem de prioridade das regras: do caso mais específico ao mais geral. */
const char* decidir_algoritmo(PerfilEntrada p) {
     /* Insertion Sort é ideal para entradas pequenas ou quase ordenadas,
       pois tem baixo overhead e aproveita a ordem existente */
    if (p.tamanho < 50) {
        return "insertion";
    }
    if (p.quase_ordenada) {
    // Se o vetor for pequeno, 5% de desordem resolve-se bem
    if (p.tamanho < 10000) {
        return "insertion";
    } 
    // Se for GIGANTE, só usamos o Insertion se a desordem for microscópica (< 0.1%)
    else if (p.desordem_estimada < 0.1) {
        return "insertion";
    }
    // Se for gigante e "mais ou menos" desordenado, fugimos para o Merge Sort!
    else {
        return "merge"; 
    }
}
    /* Counting Sort é vantajoso apenas quando a amplitude é pequena em termos
       absolutos E proporcionalmente ao tamanho, evitando desperdício de memória */
    if (p.amplitude > 0 && p.amplitude < 100000 && p.amplitude < (long long)(2 * p.tamanho)) {
        return "counting";
    }
    /* Selection Sort minimiza o número de trocas, útil quando movimentação é 
       custos. Ideal para entradas médias sem características especiais,  */
    if (p.tamanho <= 200) {
        return "selection";
    }
    /* Merge Sort lida bem com muitos duplicatas por ser estável e não
       degradar com elementos iguais, ao contrário do Heap Sort */
    if (p.densidade_duplicatas > 50.0) {
        return "merge";
    }
    /* Quick Sort garante O(n log n) em casos médios e garante constantes operacionais
       pequenos. É o mais indicado para dados grandes com distribuição aleatório por ser 
       o mais rápido */
    if (p.tamanho >= 1000) {
        return "quick";
    }

    /*Heap Sort garante sempre O(n log n) em todos os casos, prevenindo o sistema ter
      pico de memórias e tempos de execução gigantes. Ideal para ser um rede de seguração
      para o Sistema Adaptivo*/
    return "heap";
    //ATENÇÃO ARRUMAR ESSA LÓGICA, PARECE ERRADA

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
    } else if (strcmp(nome, "heap") == 0) {
        m = medir_heapSort(arr, n);
    } else if (strcmp(nome, "counting") == 0) {
        m = medir_countingSort(arr, n);
    }
    
    return m;
}