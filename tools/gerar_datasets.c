/*
 * gerar_datasets.c
 *
 * Gerador reproduzível de datasets para experimentos de ordenação.
 * Todos os tipos de entrada usados nas Partes 5 e 6 do trabalho podem
 * ser gerados aqui com seed fixo, garantindo reprodutibilidade total.
 *
 * Compilação:  gcc tools/gerar_datasets.c -o bin/gerar_datasets
 *
 * Uso:
 *   ./bin/gerar_datasets --tipo <tipo> --tamanho N [--seed S] [--desordem P] [--range R]
 *
 * Tipos disponíveis:
 *   aleatorio            Valores aleatórios em [0, 99999]
 *   quase_ordenado       Vetor ordenado com P% de trocas aleatórias (--desordem P)
 *   reverso              Decrescente estrito
 *   repetidos            Valores em [0, range-1], alta densidade de duplicatas (--range R)
 *   adversarial_counting  Força counting sort mas com distribuição extremamente concentrada
 *   adversarial_insertion Força insertion sort mas com estrutura patológica O(K*(N-K)) trocas
 *   adversarial_limiar    Amplitude exatamente no limiar do counting sort (expõe corte binário)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void swap_g(int *a, int *b) { int t = *a; *a = *b; *b = t; }

/* Fisher-Yates com seed explícito para reprodutibilidade */
static void embaralhar(int *arr, int n, unsigned int seed) {
    srand(seed);
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap_g(&arr[i], &arr[j]);
    }
}

static void imprimir(int *arr, int n) {
    for (int i = 0; i < n; i++) printf("%d\n", arr[i]);
}

int main(int argc, char *argv[]) {
    char *tipo      = NULL;
    int n           = 1000;
    unsigned seed   = 42;
    double desordem = 2.0;
    int range       = 10;

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "--tipo")     && i+1 < argc) tipo    = argv[++i];
        else if (!strcmp(argv[i], "--tamanho")  && i+1 < argc) n       = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--seed")     && i+1 < argc) seed    = (unsigned)atoi(argv[++i]);
        else if (!strcmp(argv[i], "--desordem") && i+1 < argc) desordem= atof(argv[++i]);
        else if (!strcmp(argv[i], "--range")    && i+1 < argc) range   = atoi(argv[++i]);
    }

    if (!tipo) {
        fprintf(stderr, "Uso: %s --tipo <tipo> --tamanho N [opcoes]\n\n", argv[0]);
        fprintf(stderr, "Tipos:\n");
        fprintf(stderr, "  aleatorio              valores em [0,99999], seed controlado\n");
        fprintf(stderr, "  quase_ordenado         ordenado + %%desordem de trocas (--desordem P)\n");
        fprintf(stderr, "  reverso                decrescente\n");
        fprintf(stderr, "  repetidos              valores em [0,range-1]  (--range R)\n");
        fprintf(stderr, "  adversarial_counting   forca counting sort, expoe distribuicao concentrada\n");
        fprintf(stderr, "  adversarial_insertion  forca insertion sort, estrutura patologica O(K*N)\n");
        fprintf(stderr, "  adversarial_limiar     amplitude exatamente no limiar → troca heap/counting\n");
        return 1;
    }

    srand(seed);
    int *arr = malloc(n * sizeof(int));
    if (!arr) { fprintf(stderr, "Erro de memoria\n"); return 1; }

    /* ------------------------------------------------------------------ */
    if (!strcmp(tipo, "aleatorio")) {
        for (int i = 0; i < n; i++) arr[i] = rand() % 100000;
    }

    /* ------------------------------------------------------------------ */
    else if (!strcmp(tipo, "quase_ordenado")) {
        /* Cria vetor crescente e aplica floor(n * P/100) trocas aleatórias */
        for (int i = 0; i < n; i++) arr[i] = i;
        int n_trocas = (int)(n * desordem / 100.0);
        for (int i = 0; i < n_trocas; i++) {
            int a = rand() % n, b = rand() % n;
            swap_g(&arr[a], &arr[b]);
        }
    }

    /* ------------------------------------------------------------------ */
    else if (!strcmp(tipo, "reverso")) {
        /* Decrescente: índice 0 = maior valor */
        for (int i = 0; i < n; i++) arr[i] = n - i;
    }

    /* ------------------------------------------------------------------ */
    else if (!strcmp(tipo, "repetidos")) {
        /* Valores em [0, range-1] — alta densidade de duplicatas quando range << n */
        for (int i = 0; i < n; i++) arr[i] = rand() % range;
    }

    /* ------------------------------------------------------------------ */
    else if (!strcmp(tipo, "adversarial_counting")) {
        /*
         * ADVERSARIAL 1 — "Falso Counting Sort"
         *
         * Objetivo: forçar a seleção de Counting Sort pela heurística de
         * amplitude, mas com distribuição de valores extremamente concentrada,
         * expondo que a heurística não verifica o desperdício de memória.
         *
         * Construção:
         *   - amplitude = 10*n - 2  →  amplitude < 10*n  →  Counting SELECIONADO
         *   - 99% dos valores são 0 e 1% são (amplitude_max)
         *   - O vetor de contagem aloca (amplitude_max+1) entradas ≈ 40 KB para n=5000
         *     mas utiliza apenas 2 delas (índice 0 e índice amplitude_max)
         *   - Heap Sort faria o mesmo trabalho com 0 bytes de memória extra
         *
         * Limitação exposta: a heurística considera apenas amplitude < threshold,
         * mas não verifica se os valores estão concentrados em poucos pontos.
         */
        int amp = 10 * n - 2;
        if (amp > 99998) amp = 99998;         /* respeita o limite < 100000 */
        int n_extremos = n / 100;             /* 1% de valores no extremo superior */
        for (int i = 0; i < n - n_extremos; i++) arr[i] = 0;
        for (int i = n - n_extremos; i < n; i++) arr[i] = amp;
        embaralhar(arr, n, seed + 1);         /* embaralha para não ser trivialmente ordenado */
    }

    /* ------------------------------------------------------------------ */
    else if (!strcmp(tipo, "adversarial_insertion")) {
        /*
         * ADVERSARIAL 2 — "Destruidor do Insertion Sort"
         *
         * Objetivo: enganar a métrica de desordem (inversões adjacentes) para
         * que a heurística selecione Insertion Sort em um vetor com estrutura
         * patológica que força O(K*(N-K)) movimentações.
         *
         * Construção:
         *   - K = N/10  elementos grandes crescentes no início (200000..200000+K-1)
         *   - N-K elementos pequenos crescentes no restante  (0..N-K-1)
         *   - Uma única inversão adjacente na junção (arr[K-1] > arr[K])
         *   - desordem_estimada = 1 / (N-1) ≈ 0.01%  →  quase_ordenada = true
         *   - amplitude = 200000+K-1 > 100000  →  Counting NÃO selecionado
         *   - Insertion Sort SELECIONADO pela heurística
         *
         * Comportamento do Insertion Sort:
         *   - Para cada um dos (N-K) elementos pequenos: precisa se deslocar
         *     K posições para a esquerda (passando todos os K grandes)
         *   - Total: (N-K) × K ≈ 0.9N × 0.1N = 0.09N² operações
         *   - Para N=10000: ≈ 9.000.000 trocas vs Heap Sort ≈ 133.000
         *
         * Limitação exposta: inversão adjacente não mede a MAGNITUDE de cada
         * deslocamento necessário, apenas a sua presença.
         */
        int K = n / 10;
        for (int i = 0; i < K; i++)     arr[i] = 200000 + i;    /* grandes, crescente */
        for (int i = K; i < n; i++)     arr[i] = i - K;          /* pequenos, crescente */
    }

    /* ------------------------------------------------------------------ */
    else if (!strcmp(tipo, "adversarial_limiar")) {
        /*
         * ADVERSARIAL 3 — "Limiar de Amplitude"
         *
         * Objetivo: demonstrar o corte binário arbitrário da heurística de
         * Counting Sort. Com amplitude = 10*n (exatamente no limiar), a
         * condição `amplitude < 10*n` é falsa → Heap Sort selecionado.
         * Com amplitude = 10*n - 1, seria Counting Sort.
         *
         * Construção:
         *   - Valores aleatórios em [0, 10*n], garantindo min=0 e max=10*n
         *   - amplitude = 10*n  →  `amplitude < 10*n` é FALSE  →  Heap SELECIONADO
         *   - Counting Sort com esse mesmo vetor levaria O(n + 10*n) = O(11*n)
         *   - Heap Sort leva O(n * log2(n))
         *   - Para n=5000: Counting ≈ 55.000 ops vs Heap ≈ 61.439 ops
         *   - O sistema escolhe o algoritmo ligeiramente MAIS LENTO por margem de 1 unidade
         *
         * Limitação exposta: limiares binários duros são frágeis;
         * uma diferença de 1 em amplitude muda completamente a decisão.
         */
        int amp_limiar = 10 * n;
        for (int i = 0; i < n; i++) arr[i] = rand() % (amp_limiar - 1) + 1; /* [1, amp_limiar-1] */
        arr[0] = 0;           /* garante min = 0 */
        arr[1] = amp_limiar;  /* garante max = amplitude exata do limiar */
        embaralhar(arr, n, seed + 1);
    }

    /* ------------------------------------------------------------------ */
    else {
        fprintf(stderr, "Tipo desconhecido: '%s'\n", tipo);
        free(arr);
        return 1;
    }

    imprimir(arr, n);
    free(arr);
    return 0;
}
