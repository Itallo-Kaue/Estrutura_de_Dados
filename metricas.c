#include "metricas.h"
#include <limits.h>
#include <stdlib.h>

// Função auxiliar para o qsort caso a amplitude seja grande demais
static int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

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