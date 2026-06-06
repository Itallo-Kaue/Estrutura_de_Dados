APPS    = ./apps
BIN     = ./bin
INCLUDE = ./include
OBJ     = ./obj
SRC     = ./src

all:
	gcc -c $(SRC)/metricas.c  -I $(INCLUDE)/ -o $(OBJ)/metricas.o
	gcc -c $(SRC)/execucao.c  -I $(INCLUDE)/ -o $(OBJ)/execucao.o
	gcc $(APPS)/main.c $(OBJ)/metricas.o $(OBJ)/execucao.o -I $(INCLUDE) -o $(BIN)/programa -lm

run:
	$(BIN)/programa

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/programa