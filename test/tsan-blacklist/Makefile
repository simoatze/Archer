include ../Makefile.include

CFLAGS=-g -O0 -fopenmp -fsanitize=thread 
LFLAGS=-fopenmp -fsanitize=thread $(OMP_TSAN_LIB)

all: data-races data-races-bl

data-races: data-races.c
	$(CC) $(CFLAGS) $(LFLAGS) data-races.c -o data-races

data-races-bl: data-races.c
	$(CC) $(CFLAGS) -fsanitize-blacklist=blacklist.bl $(LFLAGS) data-races.c -o data-races-bl

clean:
	rm -rf *.o data-races data-races-bl
