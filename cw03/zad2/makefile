CFLAGS=-Wall -std=c99 -I.

all: main tests clean
main: 
	gcc -o test main.c $(CFLAGS)

clean:
	rm -f *.o test
	
tests:
	./test example
	./test as
	./test


