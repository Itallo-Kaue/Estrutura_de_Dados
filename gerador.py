import random
import os

os.makedirs("datasets", exist_ok=True)

def salvar(nome, dados):
    with open(f"datasets/{nome}", "w") as f:
        f.write("\n".join(map(str, dados)) + "\n")
    print(f"Gerado: {nome} | Tamanho: {len(dados)}")

# ==========================================
# 1. ENTRADAS ALEATÓRIAS (O Caso Médio)
# ==========================================
# Testam a capacidade geral do Quick Sort e do Counting Sort.
salvar("aleatoria_1.txt", [random.randint(0, 10000) for _ in range(5000)])
salvar("aleatoria_2.txt", [random.randint(0, 50000) for _ in range(15000)])

# ==========================================
# 2. ENTRADAS QUASE ORDENADAS (O Melhor Caso)
# ==========================================
# Testam a eficiência do Insertion Sort em desordens < 5%.
quase_1 = list(range(10000))
for _ in range(50): # ~1% de desordem
    i, j = random.randint(0, 9999), random.randint(0, 9999)
    quase_1[i], quase_1[j] = quase_1[j], quase_1[i]
salvar("quase_ordenada_1.txt", quase_1)

quase_2 = list(range(20000))
for _ in range(200): # ~2% de desordem
    i, j = random.randint(0, 19999), random.randint(0, 19999)
    quase_2[i], quase_2[j] = quase_2[j], quase_2[i]
salvar("quase_ordenada_2.txt", quase_2)

# ==========================================
# 3. ENTRADAS REVERSAMENTE ORDENADAS (O Pior Caso Clássico)
# ==========================================
# Avaliam como a inversão total afeta a heurística (costuma penalizar o Insertion).
salvar("reversa_1.txt", list(range(5000, 0, -1)))
salvar("reversa_2.txt", list(range(15000, 0, -1)))

# ==========================================
# 4. ENTRADAS COM MUITOS REPETIDOS
# ==========================================
# Disparam a regra de densidade de duplicatas (>50%), ativando o Merge Sort.
salvar("repetidos_1.txt", [random.choice([10, 50, 100]) for _ in range(10000)])
salvar("repetidos_2.txt", [random.choice([1, 2, 3, 4, 5]) for _ in range(20000)])

# ==========================================
# 5. ENTRADAS PEQUENAS
# ==========================================
# Disparam o Insertion Sort devido ao N < 30 ou o Selection Sort para N <= 200.
salvar("pequena_1.txt", [random.randint(0, 100) for _ in range(15)])
salvar("pequena_2.txt", [random.randint(0, 100) for _ in range(29)])

# ==========================================
# 6. ENTRADAS GRANDES (Teste de Stress)
# ==========================================
# Testam a alocação de memória e o tempo assintótico puro (Quick Sort).
salvar("grande_1.txt", [random.randint(0, 1000000) for _ in range(100000)])
salvar("grande_2.txt", [random.randint(0, 1000000) for _ in range(250000)])

# =========================================================
# CASOS IDEAIS (Exploram o melhor caso de cada algoritmo)
# =========================================================

# 1. Insertion Sort: Quase ordenado natural (N pequeno/médio)
salvar("ideal_insertion.txt", list(range(10000))) 

# 2. Counting Sort: Amplitude igual ao tamanho (N=50000, k=50000)
salvar("ideal_counting.txt", [random.randint(0, 50000) for _ in range(50000)])

# 3. Selection Sort: Tamanho < 200, totalmente aleatório
salvar("ideal_selection.txt", random.sample(range(10000), 150))

# 4. Merge Sort: N grande, massivamente repetido (Apenas 3 valores únicos, mas com AMPLITUDE GIGANTE)
salvar("ideal_merge.txt", [random.choice([10, 5000000, 10000000]) for _ in range(50000)])

# 5. Quick Sort: N grande, aleatório perfeito, zero duplicatas
salvar("ideal_quick.txt", random.sample(range(1000000), 100000))

# 6. Heap Sort: O Fallback seguro. N=800 (Foge do Quick e do Selection)
salvar("ideal_heap.txt", random.sample(range(20000), 800))


# =========================================================
# CASOS ADVERSARIAIS (Quebram as premissas da heurística)
# =========================================================
# ==========================================
# 7. ENTRADAS ADVERSARIAIS (Os Quebradores de Heurística)
# ==========================================
# (As versões perfeitamente blindadas que fizemos no passo anterior)
salvar("adv_insertion.txt", list(range(5000, 10000)) + list(range(0, 5000)))
salvar("adv_counting.txt", random.sample(range(120000), 50000))

adv_quick = [5] * 1960 + random.sample(range(100, 500000), 2040)
random.shuffle(adv_quick)
salvar("adv_quick.txt", adv_quick)

adv_merge = [1000000] * 2000 + [9000000] * 2000
for _ in range(200):
    i, j = random.randint(0, 1999), random.randint(2000, 3999)
    adv_merge[i], adv_merge[j] = adv_merge[j], adv_merge[i]
salvar("adv_merge.txt", adv_merge)

salvar("adv_selection.txt", list(range(199000, 0, -1000)))
salvar("adv_heap.txt", list(range(999000, 0, -1000)))


