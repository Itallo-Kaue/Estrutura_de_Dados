#!/bin/bash
# executar_experimentos.sh — Automatiza compilação e execução de todos os experimentos.
# Cobre as Partes 5, 6 e 7 do enunciado.
#
# Uso:
#   chmod +x executar_experimentos.sh
#   ./executar_experimentos.sh
#
# Saída: arquivos .txt em ./resultados/

set -e

PROGRAMA="./bin/programa"
TIMEOUT_SEGUNDOS=60

echo "========================================================"
echo "  Sistema Adaptativo de Ordenação — Experimentos"
echo "========================================================"
echo ""

echo "=== [1/4] Compilando projeto principal ==="
make clean && make
echo ""

echo "=== [2/4] Compilando gerador de datasets ==="
make gerar
echo ""

mkdir -p resultados

# ── Parte 5 — Avaliação Experimental ─────────────────────
echo "=== [3/4] PARTE 5 — Avaliação Experimental (modo adaptativo) ==="

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
    "datasets/adversarial_limiar_amplitude.txt:adversarial_limiar"
)

for entrada in "${DATASETS_ADAPTATIVOS[@]}"; do
    arquivo="${entrada%%:*}"
    nome="${entrada##*:}"
    saida="resultados/adaptativo_${nome}.txt"
    echo "  -> adaptativo | $arquivo  →  $saida"
    $PROGRAMA --modo adaptativo --input "$arquivo" > "$saida"
done

echo ""

# ── Partes 6 e 7 — Comparação com algoritmos fixos ───────
echo "=== [4/4] PARTES 6 e 7 — Modo fixo (comparação obrigatória) ==="

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
    "datasets/adversarial_limiar_amplitude.txt:adversarial_limiar"
)

for alg in "${ALGORITMOS_FIXOS[@]}"; do
    for entrada in "${DATASETS_FIXOS[@]}"; do
        arquivo="${entrada%%:*}"
        nome="${entrada##*:}"
        saida="resultados/fixo_${alg}_${nome}.txt"
        echo "  -> fixo:$alg | $arquivo  →  $saida"
        timeout "${TIMEOUT_SEGUNDOS}s" $PROGRAMA --modo fixo --algoritmo "$alg" --input "$arquivo" > "$saida" \
            || echo "    [AVISO] timeout ou erro em $alg com $arquivo"
    done
done

echo ""
echo "========================================================"
echo "  Todos os experimentos concluídos."
echo "  Resultados em ./resultados/"
echo ""
echo "  Para gerar as tabelas CSV do relatório, execute:"
echo "    ./extrair_resultados.sh"
echo "========================================================"
