#define _POSIX_C_SOURCE 200809L
#include "execucao.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static double tempo_atual_ms(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

static void trocar_m(int *a, int *b, long long *trocas) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    (*trocas)++;
}

/* =========================================================
   INSERTION SORT CLÁSSICO INSTRUMENTADO
   ========================================================= */

MetricasExecucao medir_insertionSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        /* desloca elementos maiores que 'key' uma posição à frente */
        while (j >= 0) {
            m.comparacoes++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j]; /* movimentação conta como troca */
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
   INSERTION SORT 2.0 INSTRUMENTADO 
   ========================================================= */

MetricasExecucao medir_insertion2_0(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();

    for (int i = 0; i < n - 1; i++) {
        int j = i;
        while (j >= 0) {
            m.comparacoes++;
            if (arr[j] > arr[j + 1]) {
                trocar_m(&arr[j], &arr[j + 1], &m.trocas);
                j--;
            } else {
                break;
            }
        }
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   QUICK SORT INSTRUMENTADO
   ========================================================= */

static int particionar_m(int arr[], int low, int high, MetricasExecucao *m) {
    /* pivô do meio para melhor desempenho em entradas inversamente ordenadas */
    int mid = low + (high - low) / 2;
    trocar_m(&arr[mid], &arr[high], &m->trocas);

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        m->comparacoes++;
        if (arr[j] <= pivot) {
            i++;
            trocar_m(&arr[i], &arr[j], &m->trocas);
        }
    }

    trocar_m(&arr[i + 1], &arr[high], &m->trocas);
    return i + 1;
}

static void quickSort_m(int arr[], int low, int high,
                         MetricasExecucao *m, int profundidade) {
    /* contabiliza esta chamada e atualiza a profundidade máxima atingida */
    m->chamadas_recursivas++;
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;

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

static void merge_m(int arr[], int left, int mid, int right, MetricasExecucao *m) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    /* acumula a memória extra alocada para os subarrays temporários */
    m->memoria_extra_bytes += (long long)(n1 + n2) * sizeof(int);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        m->comparacoes++;
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
        m->trocas++; /* cada escrita no array original é uma movimentação */
    }

    while (i < n1) { arr[k++] = L[i++]; m->trocas++; }
    while (j < n2) { arr[k++] = R[j++]; m->trocas++; }

    free(L);
    free(R);
}

static void mergeSort_m(int arr[], int left, int right,
                         MetricasExecucao *m, int profundidade) {
    m->chamadas_recursivas++;
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;

    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort_m(arr, left, mid, m, profundidade + 1);
        mergeSort_m(arr, mid + 1, right, m, profundidade + 1);
        merge_m(arr, left, mid, right, m);
    }
}

MetricasExecucao medir_mergeSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();
    mergeSort_m(arr, 0, n - 1, &m, 1);
    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   HEAP SORT INSTRUMENTADO
   ========================================================= */

static void heapify_m(int arr[], int n, int i,
                       MetricasExecucao *m, int profundidade) {
    m->chamadas_recursivas++;
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;

    int largest = i;
    int left  = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n) {
        m->comparacoes++;
        if (arr[left] > arr[largest]) largest = left;
    }
    if (right < n) {
        m->comparacoes++;
        if (arr[right] > arr[largest]) largest = right;
    }

    if (largest != i) {
        trocar_m(&arr[i], &arr[largest], &m->trocas);
        heapify_m(arr, n, largest, m, profundidade + 1);
    }
}

MetricasExecucao medir_heapSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = false;

    double t0 = tempo_atual_ms();

    for (int i = n / 2 - 1; i >= 0; i--)
        heapify_m(arr, n, i, &m, 1);

    for (int i = n - 1; i > 0; i--) {
        trocar_m(&arr[0], &arr[i], &m.trocas);
        heapify_m(arr, i, 0, &m, 1);
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

/* =========================================================
   SHELL SORT INSTRUMENTADO
   ========================================================= */

MetricasExecucao medir_shellSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = false;

    double t0 = tempo_atual_ms();

    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap; j -= gap) {
                m.comparacoes++;
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    m.trocas++;
                } else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }

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
