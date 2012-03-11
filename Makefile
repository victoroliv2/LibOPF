LIB=./lib
INCLUDE=./include
SRC=./src
OBJ=./obj

CC=gcc

FLAGS= -march=native -O3 -Wall -fPIC -fopenmp -D NTHREADS=4 -lgomp

INCFLAGS = -I$(INCLUDE)

SRCFILES = $(shell ls ${SRC})
OBJFILES = $(SRCFILES:%.c=%.o)

all: libopf

libopf: libopf-build

create_folders:
	mkdir -p lib
	mkdir -p obj

libopf-build: \
create_folders \
aux
	$(CC) -fPIC -shared -o $(LIB)/libopf.so \
$(OBJ)/*.o

aux: $(OBJFILES)

%.o: $(SRC)/%.c $(INCLUDE)/%.h
	$(CC) $(FLAGS) $(INCFLAGS) -c $(SRC)/$*.c -o $(OBJ)/$*.o


## Cleaning-up

clean:
	rm -f $(LIB)/lib*.so; rm -f $(OBJ)/*.o

cython:
	cython libopf_py.pyx

bindings:
	$(CC) -shared -pthread -fPIC -fwrapv -O2 -Wall -fno-strict-aliasing $(INCFLAGS) -I/usr/include/python2.7 -lgomp -L$(LIB) -lopf -o $(LIB)/libopf_py.so libopf_py.c
