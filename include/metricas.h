#ifndef METRICAS_H
#define METRICAS_H

#include <stdbool.h>

typedef struct {
    int tamanho;                 
    long long amplitude;
    int repetidos;          
    double desordem_estimada_pct;  
    bool quase_ordenada;          
    double densidade_duplicatas;         
    int padroes_sequenciais;
    double tamanho_medio_runs;  
    double media_valores;       
    int tipo_distribuicao;       
} PerfilEntrada;

// Função de Análise
PerfilEntrada analisar_input(int *input, int tamanho);

// Algoritmos de Ordenação
void insertionSort(int arr[], int n);
void selectionSort(int arr[], int n);
void heapSort(int arr[], int n);
void mergeSort(int arr[], int left, int right);
void countingSort(int arr[], int n);

#endif