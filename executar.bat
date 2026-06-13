@echo off
echo A compilar o sistema...

REM Compilacao direta com o GCC (garante que funciona sem o comando make)
gcc -c src/metricas.c -I include/ -o obj/metricas.o
gcc -c src/execucao.c -I include/ -o obj/execucao.o
gcc -c src/decisao.c -I include/ -o obj/decisao.o
gcc apps/main.c obj/metricas.o obj/execucao.o obj/decisao.o -I include -o bin/programa.exe -lm

echo Compilacao concluida com sucesso!

REM Cria a pasta para guardar os resultados, caso ainda nao exista
IF NOT EXIST resultados mkdir resultados

echo ==========================================================
echo  A INICIAR BATERIA DE EXPERIMENTOS (MODO ADAPTATIVO)      
echo ==========================================================

REM Lista de todos os ficheiros de teste gerados
set datasets= adv_counting.txt adv_heap.txt adv_insertion.txt adv_merge.txt adv_quick.txt adv_selection.txt ideal_counting.txt ideal_heap.txt ideal_insertion.txt ideal_merge.txt ideal_quick.txt ideal_selection.txt aleatoria_1.txt aleatoria_2.txt quase_ordenada_1.txt quase_ordenada_2.txt reversa_1.txt reversa_2.txt repetidos_1.txt repetidos_2.txt pequena_1.txt pequena_2.txt grande_1.txt grande_2.txt
REM Loop que testa o sistema adaptativo para cada ficheiro
for %%i in (%datasets%) do (
    echo A executar Modo Adaptativo para: %%i
    bin\programa.exe --modo adaptativo --input datasets\%%i > resultados\log_adaptativo_%%i
)

echo ==========================================================
echo  A INICIAR COMPARACAO OBRIGATORIA (MODO FIXO - QUICK SORT)
echo ==========================================================

REM Loop que testa o algoritmo fixo para servir de comparacao no relatorio
for %%i in (%datasets%) do (
    echo A executar Modo Fixo (Quick Sort) para: %%i
    bin\programa.exe --modo fixo --algoritmo quick --input datasets\%%i > resultados\log_fixo_quick_%%i
)

echo ==========================================================
echo Experimentos concluidos! 
echo Os ficheiros com as metricas estao guardados na pasta "resultados".
echo ==========================================================
pause