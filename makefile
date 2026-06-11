APPS    = ./apps
BIN     = ./bin
INCLUDE = ./include
OBJ     = ./obj
SRC     = ./src

all:
	gcc -c $(SRC)/metricas.c  -I $(INCLUDE)/ -o $(OBJ)/metricas.o
	gcc -c $(SRC)/execucao.c  -I $(INCLUDE)/ -o $(OBJ)/execucao.o
	gcc -c $(SRC)/decisao.c  -I $(INCLUDE)/ -o $(OBJ)/decisao.o

	gcc $(APPS)/main.c $(OBJ)/metricas.o $(OBJ)/execucao.o $(OBJ)/decisao.o -I $(INCLUDE) -o $(BIN)/programa -lm

run:
	$(BIN)/programa --modo adaptativo --input ./datasets/aleatoria_1.txt

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/programa