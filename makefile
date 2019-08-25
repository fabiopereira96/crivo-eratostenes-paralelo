all: ep1

ep1: sieve.c
	gcc -c -Wall -Wextra -Werror -std=c99 sieve.c
	gcc -o ep1 sieve.o -lm
free:
	rm sieve.o
