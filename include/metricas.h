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
void insertion2_0(int arr[], int n);
void quickSort(int arr[], int low, int high);
void mergeSort(int arr[], int left, int right);
void heapSort(int arr[], int n);
void shellSort(int arr[], int n);

#endif