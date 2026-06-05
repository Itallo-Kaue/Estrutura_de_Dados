#include <stdio.h>
#include <stdlib.h>
#include "metricas.h" 

int main() {
    int dados[] = {10, 2, 5, 5, 2, 8, 9, 1, 100};
    
    // Calcula o tamanho da entrada
    int tamanho = sizeof(dados) / sizeof(dados[0]);
    
    PerfilEntrada perfil = analisar_input(dados, tamanho);
    
    printf("--- Perfil da Entrada Otimizado ---\n");
    printf("Tamanho: %d\n", perfil.tamanho);
    printf("Amplitude: %lld\n", perfil.amplitude);
    printf("Quantidade de Repetidos: %d\n", perfil.repetidos);
    printf("Desordem Estimada: %.2f%%\n", perfil.desordem_estimada_pct);
    printf("Tamanho Medio dos Runs: %.2f\n", perfil.tamanho_medio_runs);
    printf("Densidade de Duplicatas: %.2f%%\n", perfil.densidade_duplicatas);
    
    return 0;
}