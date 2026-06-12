#!/bin/bash
# extrair_resultados.sh
#
# Lê todos os arquivos em ./resultados/ e gera três tabelas CSV:
#   resultados/tabela_adaptativo.csv   — modo adaptativo, um dataset por linha
#   resultados/tabela_fixo.csv         — modo fixo, (algoritmo × dataset) por linha
#   resultados/tabela_adversarial.csv  — foco nos três casos adversariais
#
# Essas tabelas podem ser diretamente importadas no Excel/LibreOffice ou
# usadas para gerar gráficos com Python/R para o relatório.

PASTA="resultados"

# ---------------------------------------------------------------------------
# Funções auxiliares de extração
# ---------------------------------------------------------------------------

# Extrai um campo de um arquivo de resultado APÓS a seção "METRICAS DE EXECUCAO final"
# e ANTES de "--- Metricas de Execucao (Parte 4) ---"
extrair_campo_final() {
    local arquivo=$1
    local campo=$2
    awk '/METRICAS DE EXECUCAO final/{p=1} /Parte 4/{p=0} p && /'"$campo"'/' "$arquivo" \
        | head -1 | awk -F': ' '{gsub(/ (ms|bytes)$/,"",$2); print $2}' | tr -d ' '
}

extrair_perfil() {
    local arquivo=$1
    local campo=$2
    grep "$campo" "$arquivo" | head -1 | awk -F': ' '{print $2}' | tr -d ' '
}

extrair_algoritmo_adaptativo() {
    grep "Algoritmo selecionado pela heuristica" "$1" | awk -F': ' '{print $2}' | tr -d ' '
}

# ---------------------------------------------------------------------------
# TABELA 1 — Modo Adaptativo
# ---------------------------------------------------------------------------
CSV_ADT="$PASTA/tabela_adaptativo.csv"
echo "dataset,tamanho,algoritmo_selecionado,tempo_ms,comparacoes,trocas,memoria_bytes,desordem_pct,densidade_duplicatas" > "$CSV_ADT"

for arq in "$PASTA"/adaptativo_*.txt; do
    [ -f "$arq" ] || continue
    nome=$(basename "$arq" .txt | sed 's/^adaptativo_//')
    tam=$(extrair_perfil "$arq" "Tamanho do Vetor")
    alg=$(extrair_algoritmo_adaptativo "$arq")
    tempo=$(extrair_campo_final "$arq" "Tempo de execucao")
    comp=$(extrair_campo_final "$arq" "Comparacoes")
    trocas=$(extrair_campo_final "$arq" "Trocas")
    mem=$(extrair_campo_final "$arq" "Memoria extra")
    desordem=$(extrair_perfil "$arq" "Desordem Estimada" | tr -d '%')
    dup=$(extrair_perfil "$arq" "Densidade de Duplicatas" | tr -d '%')
    echo "$nome,$tam,$alg,$tempo,$comp,$trocas,$mem,$desordem,$dup" >> "$CSV_ADT"
done

echo "[OK] $CSV_ADT"

# ---------------------------------------------------------------------------
# TABELA 2 — Modo Fixo (Parte 7 — comparação com algoritmo fixo)
# ---------------------------------------------------------------------------
CSV_FIXO="$PASTA/tabela_fixo.csv"
echo "algoritmo_fixo,dataset,tamanho,tempo_ms,comparacoes,trocas,memoria_bytes" > "$CSV_FIXO"

for arq in "$PASTA"/fixo_*.txt; do
    [ -f "$arq" ] || continue
    base=$(basename "$arq" .txt | sed 's/^fixo_//')
    # Nome do arquivo: fixo_<algoritmo>_<dataset>.txt
    # Algoritmo é a primeira parte até o primeiro _
    alg=$(echo "$base" | cut -d_ -f1)
    nome=$(echo "$base" | cut -d_ -f2-)
    tam=$(extrair_perfil "$arq" "Tamanho do Vetor")
    tempo=$(extrair_campo_final "$arq" "Tempo de execucao")
    comp=$(extrair_campo_final "$arq" "Comparacoes")
    trocas=$(extrair_campo_final "$arq" "Trocas")
    mem=$(extrair_campo_final "$arq" "Memoria extra")
    echo "$alg,$nome,$tam,$tempo,$comp,$trocas,$mem" >> "$CSV_FIXO"
done

echo "[OK] $CSV_FIXO"

# ---------------------------------------------------------------------------
# TABELA 3 — Adversariais (foco na Parte 6)
# Compara o algoritmo selecionado pelo adaptativo com os demais no mesmo dataset
# ---------------------------------------------------------------------------
CSV_ADV="$PASTA/tabela_adversarial.csv"
echo "adversarial,tamanho,algoritmo_adaptativo,tempo_adaptativo_ms,tempo_insertion_ms,tempo_heap_ms,tempo_merge_ms,tempo_counting_ms,memoria_adaptativo_bytes" > "$CSV_ADV"

for tipo in counting insertion limiar; do
    # Encontra o arquivo adaptativo correspondente
    case $tipo in
        counting)  arq_adp="$PASTA/adaptativo_adversarial_counting.txt" ;;
        insertion) arq_adp="$PASTA/adaptativo_adversarial_insertion.txt" ;;
        limiar)    arq_adp="$PASTA/adaptativo_adversarial_limiar.txt" ;;
    esac

    [ -f "$arq_adp" ] || continue

    tam=$(extrair_perfil "$arq_adp" "Tamanho do Vetor")
    alg=$(extrair_algoritmo_adaptativo "$arq_adp")
    tempo_adp=$(extrair_campo_final "$arq_adp" "Tempo de execucao")
    mem_adp=$(extrair_campo_final "$arq_adp" "Memoria extra")

    # Extrai tempos dos algoritmos fixos para o mesmo dataset adversarial
    get_tempo_fixo() {
        local nome_arquivo="$PASTA/fixo_${1}_adversarial_${tipo}.txt"
        [ -f "$nome_arquivo" ] && extrair_campo_final "$nome_arquivo" "Tempo de execucao" || echo "N/A"
    }

    t_ins=$(get_tempo_fixo "insertion")
    t_heap=$(get_tempo_fixo "heap")
    t_merge=$(get_tempo_fixo "merge")
    t_count=$(get_tempo_fixo "counting")

    echo "$tipo,$tam,$alg,$tempo_adp,$t_ins,$t_heap,$t_merge,$t_count,$mem_adp" >> "$CSV_ADV"
done

echo "[OK] $CSV_ADV"

# ---------------------------------------------------------------------------
# TABELA 4 — Comparação Adaptativo vs. Heap Sort Fixo por dataset (Parte 7)
# ---------------------------------------------------------------------------
CSV_COMP="$PASTA/tabela_comparacao_adaptativo_vs_heap.csv"
echo "dataset,tamanho,algoritmo_adaptativo,tempo_adaptativo_ms,tempo_heap_fixo_ms,ganho_pct" > "$CSV_COMP"

for arq_adp in "$PASTA"/adaptativo_*.txt; do
    [ -f "$arq_adp" ] || continue
    nome=$(basename "$arq_adp" .txt | sed 's/^adaptativo_//')
    arq_heap="$PASTA/fixo_heap_${nome}.txt"
    [ -f "$arq_heap" ] || continue

    tam=$(extrair_perfil "$arq_adp" "Tamanho do Vetor")
    alg=$(extrair_algoritmo_adaptativo "$arq_adp")
    t_adp=$(extrair_campo_final "$arq_adp" "Tempo de execucao")
    t_heap=$(extrair_campo_final "$arq_heap" "Tempo de execucao")

    # Calcula ganho percentual do adaptativo em relação ao heap fixo
    if [ -n "$t_adp" ] && [ -n "$t_heap" ] && [ "$t_heap" != "0" ]; then
        ganho=$(awk "BEGIN{printf \"%.1f\", (($t_heap - $t_adp) / $t_heap) * 100}")
    else
        ganho="N/A"
    fi

    echo "$nome,$tam,$alg,$t_adp,$t_heap,$ganho" >> "$CSV_COMP"
done

echo "[OK] $CSV_COMP"
echo ""
echo "=== Extração concluída. Arquivos gerados em ./$PASTA/ ==="
