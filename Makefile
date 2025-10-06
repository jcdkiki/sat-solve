.PHONY: all clean

all: sat-solve sat-check

sat-solve: src/sat-solve.c src/parse.c src/parse.h
	gcc -Ofast src/sat-solve.c src/parse.c -o sat-solve

sat-check: src/sat-check.c src/parse.c src/parse.h
	gcc -Ofast src/sat-check.c src/parse.c -o sat-check

clean:
	rm -f sat-solve sat-check