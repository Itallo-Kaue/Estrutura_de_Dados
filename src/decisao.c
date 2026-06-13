#include <string.h>
#include "metricas.h"
#include "execucao.h"
#include "decisao.h"

// Seleciona o algoritmo mais adequado com base no perfil da entrada.
// Ordem de prioridade das regras: do caso mais específico ao mais geral

const char *decidir_algoritmo(PerfilEntrada p) {
    // Counting Sort: eficiente quando amplitude é proporcional ao tamanho (memória extra ≈ amplitude*4 bytes).
    // Limite 20x garante custo de memória controlado e complexidade O(n+k) ≈ O(n).    if (p.amplitude > 0 && p.amplitude < 100000 && p.amplitude < (long long)(10 * p.tamanho))
    if (p.amplitude > 0 && p.amplitude <= (long long)p.tamanho * 20)
        return "counting";

        // Insertion Sort: ideal para entradas pequenas ou genuinamente quase-ordenadas com inversões espalhadas
    // (muitos padroes_sequenciais = perturbações locais pequenas — caso ideal para insertion O(n+k)).
    // Entradas com poucos runs grandes NÃO são candidatas: cada run exige deslocamentos custosos.
    if (p.tamanho < 50 || (p.quase_ordenada && p.padroes_sequenciais > p.tamanho / 100))
        return "insertion";

    // Merge Sort: estável e eficiente com alta densidade de duplicatas (não degrada como Heap).
    // Também vantajoso quando a entrada quase-ordenada tem poucos runs grandes (une blocos em O(n)).    if (p.densidade_duplicatas > 50.0)
    if (p.densidade_duplicatas > 40.0 || (p.quase_ordenada && p.padroes_sequenciais <= p.tamanho / 100))    
        return "merge";

    // Heap Sort: O(n log n) garantido no pior caso com memória O(1), escolha segura para entradas grandes    if (p.tamanho >= 1000)
    if (p.tamanho >= 1000)    
        return "heap";

    // Selection Sort: minimiza trocas para entradas médias sem características especiais
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
