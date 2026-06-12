#!/bin/bash
# executar_experimentos.sh
#
# Automatiza compilação e execução de todos os experimentos do trabalho.
# Cobre as Partes 5, 6 e 7 do enunciado.
#
# Uso:
#   chmod +x executar_experimentos.sh
#   ./executar_experimentos.sh
#
# Saída: arquivos .txt em ./resultados/

set -e

echo "========================================================"
echo "  Sistema Adaptativo de Ordenação — Experimentos"
echo "========================================================"
echo ""

# ── Compilação ────────────────────────────────────────────
echo "=== [1/4] Compilando projeto principal ==="
make clean && make
echo ""

echo "=== [2/4] Compilando gerador de datasets ==="
make gerar
echo ""

mkdir -p resultados

PROGRAMA="./bin/programa"

# ── Parte 5 — Avaliação Experimental ─────────────────────
# Todos os tipos de entrada exigidos pelo enunciado.
echo "=== [3/4] PARTE 5 — Avaliação Experimental (modo adaptativo) ==="

DATASETS_ADAPTATIVOS=(
    # Entradas aleatórias (dois tamanhos)
    "datasets/aleatoria_1.txt:aleatorio_1"
    "datasets/aleatoria_2.txt:aleatorio_2"
    # Entradas quase ordenadas
    "datasets/quase_ordenada_1.txt:quase_ordenada_1"
    "datasets/quase_ordenada_2.txt:quase_ordenada_2"
    # Entradas reversamente ordenadas
    "datasets/reversa_1.txt:reverso_1"
    "datasets/reversa_2.txt:reverso_2"
    # Entradas com muitos elementos repetidos
    "datasets/repetidos_1.txt:repetidos_1"
    "datasets/repetidos_2.txt:repetidos_2"
    # Entradas pequenas
    "datasets/pequena_1.txt:pequena_1"
    "datasets/pequena_2.txt:pequena_2"
    # Entradas grandes
    "datasets/grande_1.txt:grande_1"
    "datasets/grande_2.txt:grande_2"
    # Parte 6 — Entradas adversariais (três casos)
    "datasets/adversarial_destruidor_insertion.txt:adversarial_insertion"
    "datasets/adversarial_falso_counting.txt:adversarial_counting"
    "datasets/adversarial_limiar_amplitude.txt:adversarial_limiar"
)

for ENTRADA in "${DATASETS_ADAPTATIVOS[@]}"; do
    ARQUIVO="${ENTRADA%%:*}"
    NOME="${ENTRADA##*:}"
    SAIDA="resultados/adaptativo_${NOME}.txt"
    echo "  -> adaptativo | $ARQUIVO  →  $SAIDA"
    $PROGRAMA --modo adaptativo --input "$ARQUIVO" > "$SAIDA"
done

echo ""

# ── Partes 6 e 7 — Comparação com algoritmos fixos ───────
# Roda cada algoritmo fixo nos datasets principais e nos adversariais.
echo "=== [4/4] PARTES 6 e 7 — Modo fixo (comparação obrigatória) ==="

ALGORITMOS_FIXOS=("insertion" "selection" "heap" "merge" "counting")

DATASETS_FIXOS=(
    # Representativos de cada tipo (evita rodar O(n²) em datasets enormes)
    "datasets/aleatoria_1.txt:aleatorio_1"
    "datasets/aleatoria_2.txt:aleatorio_2"
    "datasets/quase_ordenada_1.txt:quase_ordenada_1"
    "datasets/reversa_1.txt:reverso_1"
    "datasets/repetidos_1.txt:repetidos_1"
    "datasets/grande_1.txt:grande_1"
    # Adversariais — obrigatório pela Parte 6
    "datasets/adversarial_destruidor_insertion.txt:adversarial_insertion"
    "datasets/adversarial_falso_counting.txt:adversarial_counting"
    "datasets/adversarial_limiar_amplitude.txt:adversarial_limiar"
)

for ALG in "${ALGORITMOS_FIXOS[@]}"; do
    for ENTRADA in "${DATASETS_FIXOS[@]}"; do
        ARQUIVO="${ENTRADA%%:*}"
        NOME="${ENTRADA##*:}"
        SAIDA="resultados/fixo_${ALG}_${NOME}.txt"
        echo "  -> fixo:$ALG | $ARQUIVO  →  $SAIDA"
        # Timeout de 60s por execução — evita travar em O(n²) com datasets grandes
        timeout 60s $PROGRAMA --modo fixo --algoritmo "$ALG" --input "$ARQUIVO" > "$SAIDA" \
            || echo "    [AVISO] timeout ou erro em $ALG com $ARQUIVO"
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
