LIB=./lib
INCLUDE=./include
SRC=./src
OBJ=./obj

CC=gcc

FLAGS=-g -Wall


INCFLAGS = -I$(INCLUDE)

all: libopf

libopf: libopf-build
	echo "libopf.a built..."

libopf-build: \
aux \
$(OBJ)/opf.o \

	ar csr $(LIB)/libopf.a \
$(OBJ)/common.o \
$(OBJ)/set.o \
$(OBJ)/gqueue.o \
$(OBJ)/realheap.o \
$(OBJ)/sgctree.o \
$(OBJ)/subgraph.o \
$(OBJ)/opf.o \

$(OBJ)/opf.o: $(SRC)/opf.c
	$(CC) $(FLAGS) -c $(SRC)/opf.c $(INCFLAGS) \
	-o $(OBJ)/opf.o

aux: $(SRC)/common.c $(SRC)/set.c $(SRC)/gqueue.c $(SRC)/realheap.c $(SRC)/sgctree.c $(SRC)/subgraph.c
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/common.c -o $(OBJ)/common.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/set.c -o $(OBJ)/set.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/realheap.c -o $(OBJ)/realheap.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/sgctree.c -o $(OBJ)/sgctree.o
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/subgraph.c -o $(OBJ)/subgraph.o

## Cleaning-up

clean:
	rm -f $(LIB)/lib*.a; rm -f $(OBJ)/*.o
