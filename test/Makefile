all: data-races data-races-bl

data-races: data-races.c
	clang-archer -db data-races.c -o data-races

data-races-bl: data-races.c
	clang-archer data-races.c -o data-races-bl

clean:
	rm -rf *.o data-races data-races-bl
