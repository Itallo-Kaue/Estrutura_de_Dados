#include "metricas.h"
#include <limits.h>
#include <stdlib.h>

#define AMPLITUDE_LIMITE_FREQUENCIA 1000000

static int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// ANÁLISE DE ENTRADA: funções auxiliares ======================================================================================================================
typedef struct {
    long long min, max, soma;
    int inversoes;
    int sequencias;
} EstatisticasBasicas;

static EstatisticasBasicas calcular_estatisticas(const int *input, int n) {
    EstatisticasBasicas s;
    s.min = s.max = input[0];
    s.soma = input[0];
    s.inversoes = 0;
    s.sequencias = 1;

    bool subindo = (input[1] >= input[0]);

    for (int i = 1; i < n; i++) {
        s.soma += input[i];

        if (input[i] < s.min) s.min = input[i];
        if (input[i] > s.max) s.max = input[i];

        if (input[i - 1] > input[i])
            s.inversoes++;

        if (subindo && input[i] < input[i - 1]) {
            s.sequencias++;
            subindo = false;
        } else if (!subindo && input[i] > input[i - 1]) {
            s.sequencias++;
            subindo = true;
        }
    }

    return s;
}

static int contar_repetidos(const int *input, int n, long long amplitude, long long minimo) {
    if (amplitude < AMPLITUDE_LIMITE_FREQUENCIA) {
        int *frequencia = calloc(amplitude + 1, sizeof(int));
        if (!frequencia) return 0;

        int unicos = 0;
        for (int i = 0; i < n; i++) {
            int idx = (int)(input[i] - minimo);
            if (frequencia[idx] == 0) {
                unicos++;
                frequencia[idx] = 1;
            }
        }
        free(frequencia);
        return n - unicos;
    }

    // amplitude grande demais para array de frequência (ordena cópia para contar únicos)
    int *copia = malloc(n * sizeof(int));
    if (!copia) return 0;

    for (int i = 0; i < n; i++) copia[i] = input[i];  // cópia necessária (qsort modifica in-place)
    qsort(copia, n, sizeof(int), comparar);

    int unicos = 1;
    for (int i = 1; i < n; i++)
        if (copia[i] != copia[i - 1]) unicos++;

    free(copia);
    return n - unicos;
}

static int classificar_distribuicao(double media, long long min, long long amplitude) {
    double ponto_medio = min + amplitude / 2.0;
    double tolerancia  = amplitude * 0.10;

    if (media < ponto_medio - tolerancia) return -1;
    if (media > ponto_medio + tolerancia) return  1;
    return 0;
}

// ANÁLISE DE ENTRADA: ponto de entrada público =============================================================================================================

PerfilEntrada analisar_input(const int *input, int tamanho) {
    PerfilEntrada perfil = {0};
    perfil.tamanho = tamanho;

    if (tamanho <= 0) return perfil;

    if (tamanho == 1) {
        perfil.quase_ordenada     = true;
        perfil.tamanho_medio_runs = 1.0;
        perfil.media_valores      = input[0];
        return perfil;
    }

    EstatisticasBasicas s = calcular_estatisticas(input, tamanho);

    perfil.amplitude             = s.max - s.min;
    perfil.desordem_estimada_pct = ((double)s.inversoes / (tamanho - 1)) * 100.0;
    perfil.quase_ordenada        = perfil.desordem_estimada_pct < 5.0;
    perfil.padroes_sequenciais   = s.sequencias;
    perfil.tamanho_medio_runs    = (double)tamanho / s.sequencias;
    perfil.media_valores         = (double)s.soma / tamanho;

    perfil.repetidos             = contar_repetidos(input, tamanho, perfil.amplitude, s.min);
    perfil.densidade_duplicatas  = ((double)perfil.repetidos / tamanho) * 100.0;
    perfil.tipo_distribuicao     = classificar_distribuicao(perfil.media_valores, s.min, perfil.amplitude);

    return perfil;
}

// ALGORITMOS DE ORDENAÇÃO ======================================================================================================================================

void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j   = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx])
                min_idx = j;
        }
        if (min_idx != i)
            swap(&arr[i], &arr[min_idx]);
    }
}

static void heapify(int arr[], int n, int i) {
    int maior = i;
    int esq   = 2 * i + 1;
    int dir   = 2 * i + 2;

    if (esq < n && arr[esq] > arr[maior]) maior = esq;
    if (dir < n && arr[dir] > arr[maior]) maior = dir;

    if (maior != i) {
        swap(&arr[i], &arr[maior]);
        heapify(arr, n, maior);
    }
}

void heapSort(int arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

static void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int arr[], int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void countingSort(int arr[], int n) {
    if (n <= 1) return;

    int min = arr[0], max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }
    int range = max - min + 1;

    int *count  = calloc(range, sizeof(int));
    int *output = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)     count[arr[i] - min]++;
    for (int i = 1; i < range; i++) count[i] += count[i - 1];

    // iteração reversa garante estabilidade
    for (int i = n - 1; i >= 0; i--)
        output[--count[arr[i] - min]] = arr[i];

    for (int i = 0; i < n; i++) arr[i] = output[i];

    free(count);
    free(output);
}
