#!/bin/bash
# Script de automação de experimentos — compila e executa todos os cenários de teste

set -e

echo "=== Compilando o projeto ==="
make clean && make
echo ""

mkdir -p resultados

PROGRAMA="./bin/programa"

# -------------------------------------------------------
# MODO ADAPTATIVO — todos os tipos de entrada
# -------------------------------------------------------
echo "=== Rodando experimentos em MODO ADAPTATIVO ==="

DATASETS_ADAPTATIVOS=(
    "datasets/aleatoria_1.txt:aleatorio_1"
    "datasets/aleatoria_2.txt:aleatorio_2"
    "datasets/quase_ordenada_1.txt:quase_ordenada_1"
    "datasets/quase_ordenada_2.txt:quase_ordenada_2"
    "datasets/reversa_1.txt:reverso_1"
    "datasets/reversa_2.txt:reverso_2"
    "datasets/repetidos_1.txt:repetidos_1"
    "datasets/repetidos_2.txt:repetidos_2"
    "datasets/pequena_1.txt:pequena_1"
    "datasets/pequena_2.txt:pequena_2"
    "datasets/grande_1.txt:grande_1"
    "datasets/grande_2.txt:grande_2"
    "datasets/adversarial_destruidor_insertion.txt:adversarial_insertion"
    "datasets/adversarial_falso_counting.txt:adversarial_counting"
)

for ENTRADA in "${DATASETS_ADAPTATIVOS[@]}"; do
    ARQUIVO="${ENTRADA%%:*}"
    NOME="${ENTRADA##*:}"
    SAIDA="resultados/adaptativo_${NOME}.txt"
    echo "  -> $ARQUIVO"
    $PROGRAMA --modo adaptativo --input "$ARQUIVO" > "$SAIDA"
done

echo ""

# -------------------------------------------------------
# MODO FIXO — algoritmos fixos para comparação (Parte 7)
# Roda o dataset grande aleatório com cada algoritmo fixo
# para comparar com o sistema adaptativo
# -------------------------------------------------------
echo "=== Rodando experimentos em MODO FIXO (comparação — Parte 7) ==="

ALGORITMOS_FIXOS=("insertion" "selection" "heap" "merge" "counting")

DATASETS_FIXOS=(
    "datasets/aleatoria_1.txt:aleatorio_1"
    "datasets/aleatoria_2.txt:aleatorio_2"
    "datasets/quase_ordenada_1.txt:quase_ordenada_1"
    "datasets/reversa_1.txt:reverso_1"
    "datasets/repetidos_1.txt:repetidos_1"
    "datasets/grande_1.txt:grande_1"
    "datasets/adversarial_destruidor_insertion.txt:adversarial_insertion"
    "datasets/adversarial_falso_counting.txt:adversarial_counting"
)

for ALG in "${ALGORITMOS_FIXOS[@]}"; do
    for ENTRADA in "${DATASETS_FIXOS[@]}"; do
        ARQUIVO="${ENTRADA%%:*}"
        NOME="${ENTRADA##*:}"
        SAIDA="resultados/fixo_${ALG}_${NOME}.txt"
        echo "  -> $ALG em $ARQUIVO"
        $PROGRAMA --modo fixo --algoritmo "$ALG" --input "$ARQUIVO" > "$SAIDA"
    done
done

echo ""
echo "=== Todos os experimentos concluídos. Resultados em ./resultados/ ==="
