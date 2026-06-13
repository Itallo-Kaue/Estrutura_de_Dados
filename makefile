CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic

APPS    = ./apps
BIN     = ./bin
INCLUDE = ./include
OBJ     = ./obj
SRC     = ./src

OBJS = $(OBJ)/metricas.o $(OBJ)/execucao.o $(OBJ)/decisao.o

all: $(BIN)/programa

$(BIN)/programa: $(APPS)/main.c $(OBJS)
	$(CC) $(CFLAGS) $^ -I $(INCLUDE) -o $@ -lm

$(OBJ)/metricas.o: $(SRC)/metricas.c
	$(CC) $(CFLAGS) -c $< -I $(INCLUDE) -o $@

$(OBJ)/execucao.o: $(SRC)/execucao.c
	$(CC) $(CFLAGS) -c $< -I $(INCLUDE) -o $@

$(OBJ)/decisao.o: $(SRC)/decisao.c
	$(CC) $(CFLAGS) -c $< -I $(INCLUDE) -o $@

gerar: $(BIN)/gerar_datasets

$(BIN)/gerar_datasets: tools/gerar_datasets.c
	$(CC) $(CFLAGS) $< -o $@

run: $(BIN)/programa
	$(BIN)/programa --modo adaptativo --input ./datasets/aleatoria_1.txt

clean:
	rm -f $(OBJ)/*.o $(BIN)/programa $(BIN)/gerar_datasets