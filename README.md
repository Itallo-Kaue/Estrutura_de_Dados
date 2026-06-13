# Sistema Adaptativo de Ordenação

Trabalho prático da disciplina **SCC0606 — Estruturas de Dados II**  
Instituto de Ciências Matemáticas e de Computação — USP/ICMC  
Prof. Renato Moraes Silva

---

## Descrição

Sistema em C que analisa o perfil de uma entrada de dados e seleciona automaticamente o algoritmo de ordenação mais eficiente para aquele caso. O sistema implementa cinco algoritmos com métricas de instrumentação completas e expõe as limitações da heurística através de entradas adversariais.

---

## Estrutura do Projeto

```
.
├── apps/
│   └── main.c                  # Ponto de entrada, leitura de argumentos, exibição de resultados
├── src/
│   ├── metricas.c              # Análise de perfil + implementações dos algoritmos
│   ├── execucao.c              # Versões instrumentadas (contam comparações, trocas, tempo, memória)
│   └── decisao.c               # Heurística de seleção e dispatcher
├── include/
│   ├── metricas.h
│   ├── execucao.h
│   └── decisao.h
├── tools/
│   └── gerar_datasets.c        # Gerador reproduzível de datasets com seed fixo
├── datasets/                   # Entradas pré-geradas para os experimentos
├── resultados/                 # Saída dos experimentos (.txt e .csv)
├── graficos/                   # Gráficos gerados pelo script Python
├── makefile
├── executar_experimentos.sh    # Automatiza todas as execuções das Partes 5–7
├── extrair_resultados.sh       # Gera tabelas CSV a partir dos resultados
├── gerar_graficos.py           # Gera gráficos comparativos (matplotlib)
└── visualizacao.html           # Visualizador interativo animado dos algoritmos
```

---

## Pré-requisitos

- GCC (C99 ou superior)
- Python 3 com `matplotlib` e `pandas` (apenas para gráficos)
- Bash (para os scripts de experimentos)

---

## Compilação

```bash
# Compila o programa principal
make

# Compila o gerador de datasets
make gerar

# Remove binários e objetos
make clean
```

---

## Uso

### Modo adaptativo (heurística seleciona o algoritmo)

```bash
./bin/programa --modo adaptativo --input datasets/aleatoria_1.txt
```

### Modo fixo (algoritmo determinado manualmente)

```bash
./bin/programa --modo fixo --algoritmo heap --input datasets/aleatoria_1.txt
```

Algoritmos disponíveis: `insertion`, `selection`, `heap`, `merge`, `counting`

O programa sempre executa a **Parte 4** ao final: mede todos os algoritmos no mesmo dataset para comparação direta.

---

## Algoritmos Implementados

| Algoritmo      | Complexidade média | Memória extra | Estável |
|----------------|--------------------|---------------|---------|
| Insertion Sort | O(n²)              | O(1)          | ✓ Sim   |
| Selection Sort | O(n²)              | O(1)          | ✗ Não   |
| Heap Sort      | O(n log n)         | O(1)          | ✗ Não   |
| Merge Sort     | O(n log n)         | O(n)          | ✓ Sim   |
| Counting Sort  | O(n + k)           | O(k + n)      | ✓ Sim   |

### Heurística de Seleção

```
1. amplitude < 100.000 E amplitude < 10×n   → Counting Sort
2. tamanho < 50  OU  quase_ordenada          → Insertion Sort
3. densidade de duplicatas > 50%             → Merge Sort
4. tamanho ≥ 1.000                           → Heap Sort
5. caso geral                                → Selection Sort
```

---

## Geração de Datasets

```bash
# Aleatorio (n=5000, seed=42)
./bin/gerar_datasets --tipo aleatorio --tamanho 5000 > datasets/aleatoria_1.txt

# Quase ordenado (2% de desordem)
./bin/gerar_datasets --tipo quase_ordenado --tamanho 5000 --desordem 2 > datasets/quase.txt

# Reversamente ordenado
./bin/gerar_datasets --tipo reverso --tamanho 5000 > datasets/reverso.txt

# Muitos repetidos (10 valores distintos)
./bin/gerar_datasets --tipo repetidos --tamanho 5000 --range 10 > datasets/repetidos.txt

# Adversarial — Falso Counting Sort
./bin/gerar_datasets --tipo adversarial_counting --tamanho 5000 > datasets/adv_counting.txt

# Adversarial — Destruidor do Insertion Sort
./bin/gerar_datasets --tipo adversarial_insertion --tamanho 10000 > datasets/adv_insertion.txt

# Adversarial — Limiar de Amplitude
./bin/gerar_datasets --tipo adversarial_limiar --tamanho 5000 > datasets/adv_limiar.txt
```

---

## Entradas Adversariais

Três casos projetados para expor limitações da heurística:

### 1. Falso Counting Sort (`adversarial_counting`)
- amplitude = `10n - 2` → heurística seleciona Counting Sort
- 99% dos valores são 0, 1% no extremo máximo
- Counting aloca ~200 KB mas usa apenas 2 índices; Heap faria o mesmo com 0 bytes extras

### 2. Destruidor do Insertion Sort (`adversarial_insertion`)
- Apenas 1 inversão adjacente → `quase_ordenada = true` → Insertion Sort selecionado
- Estrutura `[grandes | pequenos]` força O(K×(N-K)) ≈ **9 milhões de deslocamentos** para N=10.000
- Heap Sort faria o mesmo em ~133.000 operações

### 3. Limiar de Amplitude (`adversarial_limiar`)
- amplitude = `10n` exatamente → `amplitude < 10n` é FALSE → Heap selecionado
- Com amplitude `10n - 1` seria Counting Sort (mais rápido neste caso)
- Demonstra a fragilidade de limiares binários duros

---

## Executar os Experimentos

```bash
# Roda todos os experimentos das Partes 5, 6 e 7
chmod +x executar_experimentos.sh
./executar_experimentos.sh

# Gera tabelas CSV a partir dos resultados
chmod +x extrair_resultados.sh
./extrair_resultados.sh

# Gera gráficos comparativos (requer matplotlib)
python3 gerar_graficos.py
```

Os CSVs gerados em `resultados/`:

| Arquivo | Conteúdo |
|---|---|
| `tabela_adaptativo.csv` | Uma linha por dataset no modo adaptativo |
| `tabela_fixo.csv` | Uma linha por (algoritmo × dataset) no modo fixo |
| `tabela_adversarial.csv` | Comparação dos 5 algoritmos nos 3 adversariais |
| `tabela_comparacao_adaptativo_vs_heap.csv` | Ganho percentual do adaptativo sobre Heap fixo |

---

## Visualizador Interativo

Abra `visualizacao.html` em qualquer navegador moderno (sem servidor necessário).

Funcionalidades:
- Animação passo a passo de todos os 5 algoritmos
- Modo adaptativo que espelha a lógica de `decisao.c`
- Pseudocódigo sincronizado com a animação
- Métricas em tempo real (comparações, trocas, tempo, memória)
- **Modo Estabilidade**: badges coloridos A/B/C rastreiam elementos com o mesmo valor e indicam em tempo real se a ordenação é ESTÁVEL ✓ ou NÃO ESTÁVEL ✗
- Todas as entradas incluindo os 3 adversariais

---

## Métricas Coletadas

Para cada execução o sistema exibe:

- Tempo de execução (ms) via `CLOCK_MONOTONIC`
- Número de comparações
- Número de trocas / movimentações
- Chamadas recursivas e profundidade máxima
- Memória extra alocada (bytes)
- Estabilidade do algoritmo
