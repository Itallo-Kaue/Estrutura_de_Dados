#define _POSIX_C_SOURCE 200809L
#include "execucao.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double tempo_atual_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1.0e6;
}

static void trocar(int *a, int *b, long long *trocas) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    (*trocas)++;
}
// Insertion Sort Instrumentado
MetricasExecucao medir_insertionSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j   = i - 1;

        while (j >= 0) {
            m.comparacoes++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
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

// Selection Sort Instrumentado
MetricasExecucao medir_selectionSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = false;

    double t0 = tempo_atual_ms();

    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;

        for (int j = i + 1; j < n; j++) {
            m.comparacoes++;
            if (arr[j] < arr[min_idx])
                min_idx = j;
        }

        if (min_idx != i)
            trocar(&arr[i], &arr[min_idx], &m.trocas);
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

// Heap Sort Instrumentado
static void heapify(int arr[], int n, int i, MetricasExecucao *m, int profundidade) {
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;

    int maior = i;
    int esq   = 2 * i + 1;
    int dir   = 2 * i + 2;

    if (esq < n) {
        m->comparacoes++;
        if (arr[esq] > arr[maior]) maior = esq;
    }
    if (dir < n) {
        m->comparacoes++;
        if (arr[dir] > arr[maior]) maior = dir;
    }

    if (maior != i) {
        trocar(&arr[i], &arr[maior], &m->trocas);
        m->chamadas_recursivas++;
        heapify(arr, n, maior, m, profundidade + 1);
    }
}

MetricasExecucao medir_heapSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = false;

    double t0 = tempo_atual_ms();

    for (int i = n / 2 - 1; i >= 0; i--) {
        m.chamadas_recursivas++;
        heapify(arr, n, i, &m, 1);
    }

    for (int i = n - 1; i > 0; i--) {
        trocar(&arr[0], &arr[i], &m.trocas);
        m.chamadas_recursivas++;
        heapify(arr, i, 0, &m, 1);
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

// Merge Sort Instrumentado
static void intercalar(int arr[], int aux[], int left, int mid, int right, MetricasExecucao *m) {
    int i = left, j = mid + 1, k = left;

    for (int idx = left; idx <= right; idx++) {
        aux[idx] = arr[idx];
        m->trocas++;
    }

    while (i <= mid && j <= right) {
        m->comparacoes++;
        arr[k++] = (aux[i] <= aux[j]) ? aux[i++] : aux[j++];
        m->trocas++;
    }

    while (i <= mid) { arr[k++] = aux[i++]; m->trocas++; }
    while (j <= right) { arr[k++] = aux[j++]; m->trocas++; }
}

static void mergeSort_rec(int arr[], int aux[], int left, int right, MetricasExecucao *m, int profundidade) {
    m->chamadas_recursivas++;
    if (profundidade > m->profundidade_maxima)
        m->profundidade_maxima = profundidade;

    if (left >= right) return;

    int mid = left + (right - left) / 2;
    mergeSort_rec(arr, aux, left, mid, m, profundidade + 1);
    mergeSort_rec(arr, aux, mid + 1, right, m, profundidade + 1);
    intercalar(arr, aux, left, mid, right, m);
}

MetricasExecucao medir_mergeSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();

    if (n > 1) {
        int *aux = malloc(n * sizeof(int));
        if (!aux) { m.tempo_ms = tempo_atual_ms() - t0; return m; }
        m.memoria_extra_bytes = (long long)n * sizeof(int);
        mergeSort_rec(arr, aux, 0, n - 1, &m, 1);
        free(aux);
    }

    m.tempo_ms = tempo_atual_ms() - t0;
    return m;
}

// Counting Sort Instrumentado
MetricasExecucao medir_countingSort(int arr[], int n) {
    MetricasExecucao m = {0};
    m.estavel = true;

    double t0 = tempo_atual_ms();

    if (n <= 1) {
        m.tempo_ms = tempo_atual_ms() - t0;
        return m;
    }

    int min = arr[0], max = arr[0];
    for (int i = 1; i < n; i++) {
    m.comparacoes++;
    if (arr[i] > max) max = arr[i];
    m.comparacoes++;
    if (arr[i] < min) min = arr[i];
}

    int range = max - min + 1;

    int *count  = calloc(range, sizeof(int));
    int *output = malloc(n * sizeof(int));
    if (!count || !output) {
        free(count); free(output);
        m.tempo_ms = tempo_atual_ms() - t0;
        return m;
    }

    m.memoria_extra_bytes = (long long)(range + n) * sizeof(int);

    for (int i = 0; i < n; i++)     count[arr[i] - min]++;
    for (int i = 1; i < range; i++) count[i] += count[i - 1];

    // iteração reversa garante estabilidade
    for (int i = n - 1; i >= 0; i--) {
        output[--count[arr[i] - min]] = arr[i];
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

// IMPRESSÃO DE MÉTRICAS

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
