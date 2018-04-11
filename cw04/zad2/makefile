CFLAGS=-Wall -std=c99 -I. -D_POSIX_C_SOURCE=199309L

all: main
main: 
	gcc -o test main.c $(CFLAGS) 
	gcc -o child child.c $(CFLAGS) 
clean:
	rm -f test child

