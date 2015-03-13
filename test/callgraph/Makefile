CC=clang
CFLAGS=-g -O0 -fopenmp -I.
LFLAGS=-L$(HOME)/usr/lib/intelomprt
DEPS = foo.h bar.h
OBJ = ompex.o foo.o bar.o

all: ompex

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@	

ompex: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o ompex
