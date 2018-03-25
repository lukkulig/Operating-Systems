CFLAGS=-Wall -std=c99 -I.
LIB=blocksarray


all: main tests clean
main: 
	gcc -o test main.c $(CFLAGS)

clean:
	rm -f *.o test
	
tests:
	./test ~/Documents \> 2016-04-16
	
	-----------------------------

	./test ~/Documents \= 2018-03-21

