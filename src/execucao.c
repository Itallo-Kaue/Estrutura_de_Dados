#define _POSIX_C_SOURCE 200809L
#include "execucao.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>


static double tempo_atual_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1.0e6;
}

static void trocar_m(int *a, int *b, long long *trocas) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    (*trocas)++;
}

/* /* =========================================================
   INSERTION SORT INSTRUMENTADO
   ========================================================= */
MetricasExecucao medir_insertionSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        /* Desloca elementos maiores que 'key' uma posição à frente */
        while (j >= 0) {
            m.comparacoes++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j]; /* Movimentação conta como troca */
                m.trocas++;
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   SELECTION SORT INSTRUMENTADO
   ========================================================= */
MetricasExecucao medir_selectionSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = false;

    double t0 = tempo_atual_ms();

    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        
        for (int j = i + 1; j < n; j++) {
            m.comparacoes++;
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        /* Só realiza a troca se o menor elemento estiver fora de posição */
        if (min_idx != i) {
            trocar_m(&arr[i], &arr[min_idx], &m.trocas);
        }
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   QUICK SORT INSTRUMENTADO (Pivô Fixo: Último Elemento)
   ========================================================= */
static int particionar_m(int arr[], int low, int high, MetricasExecucao *m) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        m->comparacoes++;
        if (arr[j] <= pivot) {
            i++;
            if (i != j) { /* Trava que evita faturamento falso de trocas */
                trocar_m(&arr[i], &arr[j], &m->trocas);
            }
        }
    }
    if ((i + 1) != high) {
        trocar_m(&arr[i + 1], &arr[high], &m->trocas);
    }
    return i + 1;
}

static void quickSort_m(int arr[], int low, int high, MetricasExecucao *m, int profundidade) {
    m->chamadas_recursivas++;
    if (profundidade > m->profundidade_maxima) {
        m->profundidade_maxima = profundidade;
    }

    if (low < high) {
        int pi = particionar_m(arr, low, high, m);
        quickSort_m(arr, low, pi - 1, m, profundidade + 1);
        quickSort_m(arr, pi + 1, high, m, profundidade + 1);
    }
}

MetricasExecucao medir_quickSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = false;

    double t0 = tempo_atual_ms();
    quickSort_m(arr, 0, n - 1, &m, 1);
    m.tempo_ms = tempo_atual_ms() - t0;
    
    return m;
}

/* =========================================================
   MERGE SORT INSTRUMENTADO 
   ========================================================= */
static void merge_m(int arr[], int aux[], int left, int mid, int right, MetricasExecucao *m) {
    int i = left;
    int j = mid + 1;
    int k = left;

    /* Transfere elementos para o array auxiliar (atribuições de suporte) */
    for (int idx = left; idx <= right; idx++) {
        aux[idx] = arr[idx];
        m->trocas++;
    }

    /* Intercalação ordenada de volta para o vetor principal */
    while (i <= mid && j <= right) {
        m->comparacoes++;
        if (aux[i] <= aux[j]) {
            arr[k++] = aux[i++];
        } else {
            arr[k++] = aux[j++];
        }
        m->trocas++;
    }

    while (i <= mid) {
        arr[k++] = aux[i++];
        m->trocas++;
    }
    while (j <= right) {
        arr[k++] = aux[j++];
        m->trocas++;
    }
}

static void mergeSort_m(int arr[], int aux[], int left, int right, MetricasExecucao *m, int profundidade) {
    m->chamadas_recursivas++;
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;

    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort_m(arr, aux, left, mid, m, profundidade + 1);
        mergeSort_m(arr, aux, mid + 1, right, m, profundidade + 1);
        merge_m(arr, aux, left, mid, right, m);
    }
}

MetricasExecucao medir_mergeSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();
    if (n > 1) {
        /* Alocação única no Heap para evitar gargalo de chamadas de sistema */
        int *aux = (int *)malloc(n * sizeof(int));
        m.memoria_extra_bytes = (long long)n * sizeof(int);

        mergeSort_m(arr, aux, 0, n - 1, &m, 1);
        free(aux);
    }
    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   COUNTING SORT INSTRUMENTADO
   ========================================================= */
MetricasExecucao medir_countingSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true; 
    m.comparacoes = 0; 

    double t0 = tempo_atual_ms();

    if (n <= 1) {
        m.tempo_ms = tempo_atual_ms() - t0;
        return m;
    }

    int max = arr[0];
    int min = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }

    int range = max - min + 1;

    int *count = (int *)calloc(range, sizeof(int));
    int *output = (int *)malloc(n * sizeof(int));

    m.memoria_extra_bytes = (long long)(range + n) * sizeof(int);

    for (int i = 0; i < n; i++) {
        count[arr[i] - min]++;
    }

    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i] - min] - 1] = arr[i];
        count[arr[i] - min]--;
        m.trocas++; 
    }

    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
        m.trocas++; 
    }

    free(count);
    free(output);

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   IMPRESSÃO DAS MÉTRICAS
   ========================================================= */

void imprimir_metricas(const char *nome, MetricasExecucao m) {
    printf("  [%s]\n", nome);
    printf("    Tempo de execucao        : %.4f ms\n",    m.tempo_ms);
    printf("    Comparacoes              : %lld\n",       m.comparacoes);
    printf("    Trocas / movimentacoes   : %lld\n",       m.trocas);
    printf("    Chamadas recursivas      : %lld\n",       m.chamadas_recursivas);
    printf("    Profundidade maxima      : %d\n",         m.profundidade_maxima);
    printf("    Memoria extra alocada    : %lld bytes\n", m.memoria_extra_bytes);
    printf("    Estavel                  : %s\n",         m.estavel ? "Sim" : "Nao");
    printf("\n");
}
