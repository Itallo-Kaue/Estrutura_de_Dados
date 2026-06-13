#ifndef EXECUCAO_H
#define EXECUCAO_H

#include <stdbool.h>

typedef struct {
    double tempo_ms;               
    long long comparacoes;         
    long long trocas;              
    int profundidade_maxima;       
    long long chamadas_recursivas; 
    long long memoria_extra_bytes; 
    bool estavel;                  
} MetricasExecucao;


MetricasExecucao medir_insertionSort(int arr[], int n);
MetricasExecucao medir_selectionSort(int arr[], int n)
MetricasExecucao medir_quickSort(int arr[], int n);
MetricasExecucao medir_mergeSort(int arr[], int n);
MetricasExecucao medir_heapSort(int arr[], int n);
MetricasExecucao medir_countingSort(int arr[], int n);

void imprimir_metricas(const char *nome, MetricasExecucao m);

#endif
