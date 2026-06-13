/*
 * gerar_datasets.c — Gerador reproduzível de datasets para experimentos de ordenação.
 *
 * Compilação:  gcc tools/gerar_datasets.c -o bin/gerar_datasets
 *
 * Uso:
 *   ./bin/gerar_datasets --tipo <tipo> --tamanho N [--seed S] [--desordem P] [--range R]
 *
 * Tipos disponíveis:
 *   aleatorio             Valores aleatórios em [0, 99999]
 *   quase_ordenado        Vetor ordenado com P% de trocas aleatórias (--desordem P)
 *   reverso               Decrescente estrito
 *   repetidos             Valores em [0, range-1], alta densidade de duplicatas (--range R)
 *   adversarial_counting  Força counting sort mas com distribuição extremamente concentrada
 *   adversarial_insertion Força insertion sort mas com estrutura patológica O(K*(N-K)) trocas
 *   adversarial_limiar    Amplitude exatamente no limiar do counting sort (expõe corte binário)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================
//                  UTILITÁRIOS 
// =============================================
static void trocar(int *a, int *b) { int t = *a; *a = *b; *b = t; }

/* Fisher-Yates com seed explícito para reprodutibilidade */
static void embaralhar(int *arr, int n, unsigned int seed) {
    srand(seed);
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        trocar(&arr[i], &arr[j]);
    }
}

static void imprimir(const int *arr, int n) {
    for (int i = 0; i < n; i++) printf("%d\n", arr[i]);
}

// =============================================
//          GERADORES POR TIPO 
// =============================================

static void gerar_aleatorio(int *arr, int n) {
    for (int i = 0; i < n; i++) arr[i] = rand() % 100000;
}

static void gerar_quase_ordenado(int *arr, int n, double desordem_pct) {
    for (int i = 0; i < n; i++) arr[i] = i;
    int n_trocas = (int)(n * desordem_pct / 100.0);
    for (int i = 0; i < n_trocas; i++) {
        int a = rand() % n, b = rand() % n;
        trocar(&arr[a], &arr[b]);
    }
}

static void gerar_reverso(int *arr, int n) {
    for (int i = 0; i < n; i++) arr[i] = n - i;
}

static void gerar_repetidos(int *arr, int n, int range) {
    for (int i = 0; i < n; i++) arr[i] = rand() % range;
}

// objetivo: forçar Counting Sort pela amplitude, mas com 99% dos valores concentrados em apenas dois pontos
// expõe que a heurística não verifica desperdício de memória (array de contagem usa ~200 KB, mas só 2 índices ativos)
// amplitude = 10*n - 2  →  < 10*n  →  Counting SELECIONADO
static void gerar_adversarial_counting(int *arr, int n, unsigned int seed) {
    int amp = 10 * n - 2;
    if (amp > 99998) amp = 99998;
    int n_extremos = n / 100;
    for (int i = 0; i < n - n_extremos; i++) arr[i] = 0;
    for (int i = n - n_extremos; i < n; i++) arr[i] = amp;
    embaralhar(arr, n, seed + 1);
}

// objetivo: enganar a métrica de desordem (só 1 inversão adjacente) para que Insertion Sort seja selecionado, mas forçar O(K*(N-K)) deslocamentos.
// [200000..200000+K-1] crescente | [0..N-K-1] crescente
//  desordem = 1/(N-1) ≈ 0.01%  → quase_ordenada = true  → Insertion SELECIONADO
//  amplitude > 100000           → Counting NÃO selecionado
//  (N-K)×K ≈ 9.000.000 trocas para N=10000 vs Heap ≈ 133.000
static void gerar_adversarial_insertion(int *arr, int n) {
    int K = n / 10;
    for (int i = 0; i < K; i++) arr[i] = 200000 + i;
    for (int i = K; i < n; i++) arr[i] = i - K;
}

static void gerar_adversarial_limiar(int *arr, int n, unsigned int seed) {
    /*
     * Objetivo: demonstrar a fragilidade do corte binário da heurística.
     * amplitude = 10*n  →  `amplitude < 10*n` é FALSE  →  Heap SELECIONADO
     * amplitude = 10*n - 1  →  Counting seria selecionado e seria mais rápido.
     * Diferença de 1 unidade muda completamente a decisão.
     */
    int amp_limiar = 10 * n;
    for (int i = 0; i < n; i++) arr[i] = rand() % (amp_limiar - 1) + 1;
    arr[0] = 0;
    arr[1] = amp_limiar;
    embaralhar(arr, n, seed + 1);
}

// =============================================
//                   MAIN  
// =============================================


static void exibir_uso(const char *prog) {
    fprintf(stderr, "Uso: %s --tipo <tipo> --tamanho N [opcoes]\n\n", prog);
    fprintf(stderr, "Tipos:\n");
    fprintf(stderr, "  aleatorio              valores em [0,99999], seed controlado\n");
    fprintf(stderr, "  quase_ordenado         ordenado + %%desordem de trocas (--desordem P)\n");
    fprintf(stderr, "  reverso                decrescente\n");
    fprintf(stderr, "  repetidos              valores em [0,range-1]  (--range R)\n");
    fprintf(stderr, "  adversarial_counting   forca counting sort, expoe distribuicao concentrada\n");
    fprintf(stderr, "  adversarial_insertion  forca insertion sort, estrutura patologica O(K*N)\n");
    fprintf(stderr, "  adversarial_limiar     amplitude exatamente no limiar → troca heap/counting\n");
}

int main(int argc, char *argv[]) {
    const char *tipo = NULL;
    int      n        = 1000;
    unsigned seed     = 42;
    double   desordem = 2.0;
    int      range    = 10;

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "--tipo")     && i+1 < argc) tipo    = argv[++i];
        else if (!strcmp(argv[i], "--tamanho")  && i+1 < argc) n       = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--seed")     && i+1 < argc) seed    = (unsigned)atoi(argv[++i]);
        else if (!strcmp(argv[i], "--desordem") && i+1 < argc) desordem= atof(argv[++i]);
        else if (!strcmp(argv[i], "--range")    && i+1 < argc) range   = atoi(argv[++i]);
    }

    if (!tipo) { exibir_uso(argv[0]); return 1; }

    srand(seed);
    int *arr = malloc(n * sizeof(int));
    if (!arr) { fprintf(stderr, "Erro de memoria\n"); return 1; }

    if      (!strcmp(tipo, "aleatorio"))             gerar_aleatorio(arr, n);
    else if (!strcmp(tipo, "quase_ordenado"))         gerar_quase_ordenado(arr, n, desordem);
    else if (!strcmp(tipo, "reverso"))                gerar_reverso(arr, n);
    else if (!strcmp(tipo, "repetidos"))              gerar_repetidos(arr, n, range);
    else if (!strcmp(tipo, "adversarial_counting"))   gerar_adversarial_counting(arr, n, seed);
    else if (!strcmp(tipo, "adversarial_insertion"))  gerar_adversarial_insertion(arr, n);
    else if (!strcmp(tipo, "adversarial_limiar"))     gerar_adversarial_limiar(arr, n, seed);
    else {
        fprintf(stderr, "Tipo desconhecido: '%s'\n", tipo);
        free(arr);
        return 1;
    }

    imprimir(arr, n);
    free(arr);
    return 0;
}
