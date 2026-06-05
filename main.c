#include <stdio.h>
#include <stdlib.h>
#include "metricas.h" 

int main() {
    int dados[] = {10, 2, 5, 5, 2, 8, 9, 1, 100};
    int tamanho = sizeof(dados) / sizeof(dados[0]);
    
    // Extrair as métricas
    PerfilEntrada perfil = analisar_input(dados, tamanho);
    
    printf("--- Perfil da Entrada Otimizado ---\n");
    printf("Tamanho: %d\n", perfil.tamanho);
    printf("Amplitude: %lld\n", perfil.amplitude);
    printf("Quantidade de Repetidos: %d\n", perfil.repetidos);
    printf("Desordem Estimada: %.2f%%\n", perfil.desordem_estimada_pct);
    printf("Tamanho Medio dos Runs: %.2f\n", perfil.tamanho_medio_runs);
    printf("Densidade de Duplicatas: %.2f%%\n", perfil.densidade_duplicatas);
    
    //Testar uma ordenação (Exemplo: Quick Sort)
    printf("\nArray original: ");
    for(int i = 0; i < tamanho; i++) printf("%d ", dados[i]);
    printf("\n");

    quickSort(dados, 0, tamanho - 1);

    printf("Array ordenado: ");
    for(int i = 0; i < tamanho; i++) printf("%d ", dados[i]);
    printf("\n");

    return 0;
}