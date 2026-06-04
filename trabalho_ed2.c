#include <stdio.h>
#include <stdlib.h>

// Estrutura para armazenar o perfil da entrada
typedef struct {
    int tamanho;
    int amplitude;
    int repetidos;
    float desordem_estimada_pct;
} PerfilEntrada;

// 1. Amplitude dos Valores
int obter_amplitude(int arr[], int n) {
    if (n <= 0) return 0;
    
    int min = arr[0];
    int max = arr[0];
    
    for (int i = 1; i < n; i++) {
        if (arr[i] < min) min = arr[i];
        if (arr[i] > max) max = arr[i];
    }
    
    return max - min;
}

// 2. Percentual Estimado de Desordem
float estimar_desordem(int arr[], int n) {
    if (n <= 1) return 0.0;
    
    int pares_desordenados = 0;
    
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            pares_desordenados++;
        }
    }
    
    return ((float)pares_desordenados / (n - 1)) * 100.0;
}

// Função auxiliar para o qsort
int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// 3. Quantidade de Valores Repetidos
int obter_quantidade_repetidos(int arr[], int n) {
    if (n <= 1) return 0;

    // Aloca memória para a cópia do array
    int *copia = (int *)malloc(n * sizeof(int));
    if (copia == NULL) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }

    // Copia os dados
    for (int i = 0; i < n; i++) {
        copia[i] = arr[i];
    }

    // Ordena a cópia
    qsort(copia, n, sizeof(int), comparar);

    // Conta os valores únicos
    int valores_unicos = 1;
    for (int i = 1; i < n; i++) {
        if (copia[i] != copia[i - 1]) {
            valores_unicos++;
        }
    }

    // Libera a memória da cópia
    free(copia);
    
    // Total de itens menos os únicos dá a quantidade de repetições
    return n - valores_unicos;
}

// Função integradora que monta o perfil
PerfilEntrada analisar_entrada(int arr[], int n) {
    PerfilEntrada perfil;
    
    perfil.tamanho = n;
    perfil.amplitude = obter_amplitude(arr, n);
    perfil.repetidos = obter_quantidade_repetidos(arr, n);
    perfil.desordem_estimada_pct = estimar_desordem(arr, n);
    
    return perfil;
}

// Teste integrado
int main() {
    int dados[] = {10, 2, 5, 5, 2, 8, 9, 1, 100};
    
    // Em C, calculamos o tamanho do array estático dividindo o tamanho 
    // total em bytes pelo tamanho de um único elemento.
    int n = sizeof(dados) / sizeof(dados[0]);
    
    PerfilEntrada perfil = analisar_entrada(dados, n);
    
    printf("--- Perfil da Entrada ---\n");
    printf("Tamanho: %d\n", perfil.tamanho);
    printf("Amplitude: %d\n", perfil.amplitude);
    printf("Quantidade de Repetidos: %d\n", perfil.repetidos);
    printf("Desordem Estimada: %.2f%%\n", perfil.desordem_estimada_pct);
    
    return 0;
}