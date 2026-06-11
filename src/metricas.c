#include "metricas.h"
#include <limits.h>
#include <stdlib.h>

/* =========================================================
   FUNÇÕES AUXILIARES (Encapsuladas com 'static')
   ========================================================= */

static int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* =========================================================
   ANÁLISE DE ENTRADA
   ========================================================= */

PerfilEntrada analisar_input(int *input, int tamanho) {
    PerfilEntrada metrics = {0};
    metrics.tamanho = tamanho; 

    if (tamanho <= 0) return metrics;

    long long minimo = input[0];
    long long maximo = input[0];

    if (tamanho == 1) {
        metrics.quase_ordenada = true;
        metrics.tamanho_medio_runs = 1.0;
        metrics.media_valores = input[0];
        metrics.tipo_distribuicao = 0; 
        return metrics;
    }
    
    int inversoes_adjacentes = 0;
    int sequencias_ordenadas = 1; 
    bool subindo = input[1] >= input[0];
    long long soma_total = input[0];

    /* O seguinte for pega o valor máximo e mínimo do input, calcula
       quantas vezes a sequência dentro dele é mantida ou é desmanchada
       e pega o valor total de dentro do array do input.
       -----Primeira passagem O(n)-----
    */

    for (int i = 1; i < tamanho; i++) {
        soma_total += input[i];

        if (input[i] < minimo) minimo = input[i];
        if (input[i] > maximo) maximo = input[i];

        if (input[i - 1] > input[i]) {
            inversoes_adjacentes++;  
        }
        
        if (subindo && input[i] < input[i - 1]) {
            sequencias_ordenadas++;
            subindo = false;
        } else if (!subindo && input[i] > input[i - 1]) {
            sequencias_ordenadas++;
            subindo = true;
        }
    }

    metrics.amplitude = maximo - minimo;
    metrics.desordem_estimada_pct = ((double)inversoes_adjacentes / (tamanho - 1)) * 100.0;
    metrics.quase_ordenada = (metrics.desordem_estimada_pct < 5.0);
    metrics.padroes_sequenciais = sequencias_ordenadas; 
    metrics.tamanho_medio_runs = (double)tamanho / metrics.padroes_sequenciais;
    metrics.media_valores = (double)soma_total / tamanho;

    /*Estratégia mista para contar valores repetidos, se o valor da amplitude
      for razoável (< 1 milhão), usaremos um array de frequência (O(n))
    */
    if (metrics.amplitude < 1000000) {
        int *frequencia = calloc(metrics.amplitude + 1, sizeof(int));
        if (frequencia != NULL) {
            int unicos = 0;
            for (int i = 0; i < tamanho; i++) {
                int indice = input[i] - minimo;
                if (frequencia[indice] == 0) {
                    unicos++;
                    frequencia[indice] = 1; 
                }
            }
            metrics.repetidos = tamanho - unicos;
            free(frequencia);
        }
    } else {
    /*// Fallback: Se a amplitude for imensa, usamos o qsort
         pra não estourar a memória (O(nlogn))
    */
       int *copia = malloc(tamanho * sizeof(int));
        if (copia != NULL) {
            for (int i = 0; i < tamanho; i++) copia[i] = input[i];
            qsort(copia, tamanho, sizeof(int), comparar);
            
            int unicos = 1;
            for (int i = 1; i < tamanho; i++) {
                if (copia[i] != copia[i - 1]) unicos++;
            }
            metrics.repetidos = tamanho - unicos;
            free(copia);
        }
    }
    metrics.densidade_duplicatas = ((double)metrics.repetidos / tamanho)*100;
    
    // Análise do tipo de distribuição
    double ponto_medio_amplitude = minimo + (metrics.amplitude / 2.0);
    double tolerancia = metrics.amplitude * 0.10;

    if (metrics.media_valores < (ponto_medio_amplitude - tolerancia)) {
        metrics.tipo_distribuicao = -1; // Dados concentrados nos valores menores
    } else if (metrics.media_valores > (ponto_medio_amplitude + tolerancia)) {
        metrics.tipo_distribuicao = 1;  // Dados concentrados nos valores maiores
    } else {
        metrics.tipo_distribuicao = 0;  // Distribuição equilibrada/uniforme
    }
    
    return metrics;
}

/* =========================================================
   ALGORITMOS DE ORDENAÇÃO
   ========================================================= */



/* INSERTION SORT */

void insertionSort(int arr[], int n)
{
    /* insertion sort classico*/
    int i, j, key;

    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

/* SELECTION SORT CONVENCIONAL */
void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            swap(&arr[i], &arr[min_idx]);
        }
    }
}

/* HEAP SORT */

static void heapify(int arr[], int n, int i) {
    int largest = i; // pai
    int l = 2 * i + 1; // filhos esquerdo e direito
    int r = 2 * i + 2;

    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;

    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
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

/* MERGE SORT */

void merge(int arr[], int left, int mid, int right)
{
    /* Recebe duas particoes adjacentes ja ordenadas e as combina em uma unica particao maior, mantendo a ordenacao.*/
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];

    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int arr[], int left, int right)
{
    /* aplicacao da funcao merge*/
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

/* COUNTING SORT CONVENCIONAL */
void countingSort(int arr[], int n) {
    if (n <= 1) return;

    int max = arr[0];
    int min = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }
    int range = max - min + 1;

    int *count = (int *)calloc(range, sizeof(int));
    int *output = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) count[arr[i] - min]++;
    for (int i = 1; i < range; i++) count[i] += count[i - 1];
    
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i] - min] - 1] = arr[i];
        count[arr[i] - min]--;
    }
    for (int i = 0; i < n; i++) arr[i] = output[i];

    free(count);
    free(output);
}