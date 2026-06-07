#include <stdio.h>
#include "metricas.h"
#include "execucao.h"

// protótipos das suas funções
const char* decidir_algoritmo(PerfilEntrada p);
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n);

int main() {
    // teste
    int dados[] = {15, 3, 12, 6, 9, 1, 10, 8, 4, 7, 2, 5, 11, 14, 13};
    int n = sizeof(dados) / sizeof(dados[0]);

    printf("=== SISTEMA ADAPTATIVO: TESTE DE INTEGRACAO ===\n");

    // analise 
    PerfilEntrada perf = analisar_input(dados, n);

    // decisao
    const char* algoritmo_escolhido = decidir_algoritmo(perf);
    printf("Algoritmo selecionado pela heuristica: %s\n", algoritmo_escolhido);

    // execucao e coleta de dados
    MetricasExecucao m = executar_com_inteligencia(algoritmo_escolhido, dados, n);

    // resultado
    printf("\nResultado da Execucao:\n");
    imprimir_metricas(algoritmo_escolhido, m);

    return 0;
}