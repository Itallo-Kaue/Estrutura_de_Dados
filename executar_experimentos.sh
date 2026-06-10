#!/bin/bash
echo "Iniciando bateria de testes automatizados..."

# Criar a pasta de resultados se não existir
mkdir -p resultados

# Executar o programa passando diferentes datasets criados pelo grupo
./bin/programa --modo adaptativo --input datasets/aleatorio_1000.txt > resultados/adaptativo_aleatorio_1000.txt
./bin/programa --modo adaptativo --input datasets/invertido_1000.txt > resultados/adaptativo_invertido_1000.txt
./bin/programa --modo adaptativo --input datasets/adversarial.txt > resultados/adaptativo_adversarial.txt

echo "Testes concluídos. Resultados salvos na pasta /resultados."