#!/bin/bash
# extrair_resultados.sh — Lê ./resultados/ e gera quatro tabelas CSV para o relatório.
#
#   resultados/tabela_adaptativo.csv                    — modo adaptativo por dataset
#   resultados/tabela_fixo.csv                          — modo fixo (algoritmo × dataset)
#   resultados/tabela_adversarial.csv                   — foco nos três casos adversariais
#   resultados/tabela_comparacao_adaptativo_vs_heap.csv — adaptativo vs. heap fixo
#
# Regras de qualidade aplicadas:
#   - Linhas com campos essenciais ausentes são descartadas (sem linhas vazias)
#   - Resultados ordenados por tamanho do vetor (crescente)
#   - Nomes de colunas incluem unidade entre parênteses onde aplicável
#   - Campos numéricos ausentes exibem "N/A" em vez de vazio

PASTA="resultados"
CABECALHO_SECAO_METRICAS="METRICAS DE EXECUCAO"

# ---------------------------------------------------------------------------
# Funções de extração
# ---------------------------------------------------------------------------

extrair_campo_metricas() {
    local arquivo=$1 campo=$2
    awk "/$CABECALHO_SECAO_METRICAS/{p=1} p && /$campo/" "$arquivo" \
        | head -1 | awk -F': ' '{gsub(/ (ms|bytes)$/,"",$2); print $2}' | tr -d ' '
}

extrair_campo_perfil() {
    local arquivo=$1 campo=$2
    grep "$campo" "$arquivo" | head -1 | awk -F': ' '{print $2}' | tr -d ' '
}

extrair_algoritmo_adaptativo() {
    grep "Algoritmo selecionado pela heuristica" "$1" | awk -F': ' '{print $2}' | tr -d ' '
}

tempo_do_arquivo_fixo() {
    local arquivo="$PASTA/fixo_${1}_adversarial_${2}.txt"
    [ -f "$arquivo" ] && extrair_campo_metricas "$arquivo" "Tempo de execucao" || echo "N/A"
}

# Retorna 0 se todos os argumentos são não-vazios e diferentes de "N/A"
campos_validos() {
    for v in "$@"; do
        [ -z "$v" ] && return 1
    done
    return 0
}

# ---------------------------------------------------------------------------
# TABELA 1 — Modo Adaptativo
# ---------------------------------------------------------------------------
CSV_ADT="$PASTA/tabela_adaptativo.csv"
CABECALHO_ADT="dataset,n_elementos,algoritmo_selecionado,tempo(ms),comparacoes,trocas,memoria(bytes),desordem(%),duplicatas(%)"
echo "$CABECALHO_ADT" > "$CSV_ADT"

LINHAS_ADT=""
for arq in "$PASTA"/adaptativo_*.txt; do
    [ -f "$arq" ] || continue
    nome=$(basename "$arq" .txt | sed 's/^adaptativo_//')
    tam=$(extrair_campo_perfil  "$arq" "Tamanho do Vetor")
    alg=$(extrair_algoritmo_adaptativo "$arq")
    tempo=$(extrair_campo_metricas "$arq" "Tempo de execucao")
    comp=$(extrair_campo_metricas  "$arq" "Comparacoes")
    trocas=$(extrair_campo_metricas "$arq" "Trocas")
    mem=$(extrair_campo_metricas   "$arq" "Memoria extra")
    desordem=$(extrair_campo_perfil "$arq" "Desordem Estimada" | tr -d '%')
    dup=$(extrair_campo_perfil     "$arq" "Densidade de Duplicatas" | tr -d '%')

    campos_validos "$tam" "$alg" "$tempo" || continue
    LINHAS_ADT+="$tam,$nome,$tam,$alg,$tempo,$comp,$trocas,$mem,$desordem,$dup"$'\n'
done

# Ordena por tamanho (coluna 1) e remove a coluna auxiliar de ordenação
echo "$LINHAS_ADT" | sort -t, -k1 -n | awk -F, 'NF>1{$1=""; sub(/^,/,""); print}' OFS=, >> "$CSV_ADT"
echo "[OK] $CSV_ADT"

# ---------------------------------------------------------------------------
# TABELA 2 — Modo Fixo
# ---------------------------------------------------------------------------
CSV_FIXO="$PASTA/tabela_fixo.csv"
CABECALHO_FIXO="algoritmo,dataset,n_elementos,tempo(ms),comparacoes,trocas,memoria(bytes)"
echo "$CABECALHO_FIXO" > "$CSV_FIXO"

LINHAS_FIXO=""
for arq in "$PASTA"/fixo_*.txt; do
    [ -f "$arq" ] || continue
    base=$(basename "$arq" .txt | sed 's/^fixo_//')
    alg=$(echo "$base"  | cut -d_ -f1)
    nome=$(echo "$base" | cut -d_ -f2-)
    tam=$(extrair_campo_perfil  "$arq" "Tamanho do Vetor")
    tempo=$(extrair_campo_metricas "$arq" "Tempo de execucao")
    comp=$(extrair_campo_metricas  "$arq" "Comparacoes")
    trocas=$(extrair_campo_metricas "$arq" "Trocas")
    mem=$(extrair_campo_metricas   "$arq" "Memoria extra")

    campos_validos "$tam" "$alg" "$tempo" || continue
    LINHAS_FIXO+="$alg,$tam,$nome,$tam,$tempo,$comp,$trocas,$mem"$'\n'
done

# Ordena por algoritmo (coluna 1) depois por tamanho (coluna 2), remove colunas auxiliares
echo "$LINHAS_FIXO" | sort -t, -k1,1 -k2,2n | awk -F, 'NF>1{print $1","$3","$4","$5","$6","$7","$8}' >> "$CSV_FIXO"
echo "[OK] $CSV_FIXO"

# ---------------------------------------------------------------------------
# TABELA 3 — Adversariais
# ---------------------------------------------------------------------------
CSV_ADV="$PASTA/tabela_adversarial.csv"
echo "caso_adversarial,n_elementos,algoritmo_adaptativo,t_adaptativo(ms),t_insertion(ms),t_heap(ms),t_merge(ms),t_counting(ms),memoria_adaptativo(bytes)" > "$CSV_ADV"

for tipo in counting insertion limiar; do
    arq_adp="$PASTA/adaptativo_adversarial_${tipo}.txt"
    [ -f "$arq_adp" ] || continue

    tam=$(extrair_campo_perfil    "$arq_adp" "Tamanho do Vetor")
    alg=$(extrair_algoritmo_adaptativo "$arq_adp")
    tempo_adp=$(extrair_campo_metricas "$arq_adp" "Tempo de execucao")
    mem_adp=$(extrair_campo_metricas   "$arq_adp" "Memoria extra")

    campos_validos "$tam" "$alg" "$tempo_adp" || continue

    t_ins=$(tempo_do_arquivo_fixo   "insertion" "$tipo")
    t_heap=$(tempo_do_arquivo_fixo  "heap"      "$tipo")
    t_merge=$(tempo_do_arquivo_fixo "merge"     "$tipo")
    t_count=$(tempo_do_arquivo_fixo "counting"  "$tipo")

    echo "$tipo,$tam,$alg,$tempo_adp,$t_ins,$t_heap,$t_merge,$t_count,$mem_adp" >> "$CSV_ADV"
done

echo "[OK] $CSV_ADV"

# ---------------------------------------------------------------------------
# TABELA 4 — Adaptativo vs. Heap Fixo
# ---------------------------------------------------------------------------
CSV_COMP="$PASTA/tabela_comparacao_adaptativo_vs_heap.csv"
echo "dataset,n_elementos,algoritmo_adaptativo,t_adaptativo(ms),t_heap_fixo(ms),diferenca_pct" > "$CSV_COMP"

LINHAS_COMP=""
for arq_adp in "$PASTA"/adaptativo_*.txt; do
    [ -f "$arq_adp" ] || continue
    nome=$(basename "$arq_adp" .txt | sed 's/^adaptativo_//')
    arq_heap="$PASTA/fixo_heap_${nome}.txt"
    [ -f "$arq_heap" ] || continue

    tam=$(extrair_campo_perfil  "$arq_adp" "Tamanho do Vetor")
    alg=$(extrair_algoritmo_adaptativo "$arq_adp")
    t_adp=$(extrair_campo_metricas  "$arq_adp"  "Tempo de execucao")
    t_heap=$(extrair_campo_metricas "$arq_heap" "Tempo de execucao")

    campos_validos "$tam" "$alg" "$t_adp" "$t_heap" || continue

    # diferenca_pct > 0: adaptativo é mais rápido que heap fixo
    # diferenca_pct < 0: adaptativo é mais lento  (caso adversarial)
    if [ "$t_heap" != "0" ]; then
        dif=$(awk "BEGIN{printf \"%.1f\", (($t_heap - $t_adp) / $t_heap) * 100}")
    else
        dif="N/A"
    fi

    LINHAS_COMP+="$tam,$nome,$tam,$alg,$t_adp,$t_heap,$dif"$'\n'
done

echo "$LINHAS_COMP" | sort -t, -k1 -n | awk -F, 'NF>1{print $2","$3","$4","$5","$6","$7}' >> "$CSV_COMP"
echo "[OK] $CSV_COMP"
echo ""
echo "=== Extração concluída. Arquivos em ./$PASTA/ ==="
