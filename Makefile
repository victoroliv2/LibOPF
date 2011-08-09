LIB=./lib
INCLUDE=./include
SRC=./src
OBJ=./obj

CC=gcc

FLAGS= -O3 -Wall -fPIC


INCFLAGS = -I$(INCLUDE)

all: libopf

libopf: libopf-build

libopf-build: \
aux

	$(CC) -fPIC -shared -o $(LIB)/libopf.so \
$(OBJ)/common.o \
$(OBJ)/set.o \
$(OBJ)/realheap.o \
$(OBJ)/linearalloc.o \
$(OBJ)/metrics.o \
$(OBJ)/measures.o \
$(OBJ)/subgraph.o \
$(OBJ)/knn.o \
$(OBJ)/supervised.o \
$(OBJ)/unsupervised.o

aux: $(SRC)/common.c $(SRC)/set.c $(SRC)/realheap.c $(SRC)/linearalloc.c  $(SRC)/metrics.c  $(SRC)/measures.c $(SRC)/subgraph.c $(SRC)/knn.c $(SRC)/supervised.c $(SRC)/unsupervised.c
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/common.c       -o $(OBJ)/common.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/set.c          -o $(OBJ)/set.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/realheap.c     -o $(OBJ)/realheap.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/linearalloc.c  -o $(OBJ)/linearalloc.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/metrics.c      -o $(OBJ)/metrics.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/measures.c     -o $(OBJ)/measures.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/subgraph.c     -o $(OBJ)/subgraph.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/knn.c          -o $(OBJ)/knn.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/supervised.c   -o $(OBJ)/supervised.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/unsupervised.c -o $(OBJ)/unsupervised.o

## Cleaning-up

clean:
	rm -f $(LIB)/lib*.so; rm -f $(OBJ)/*.o
