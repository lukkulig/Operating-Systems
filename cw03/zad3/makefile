CFLAGS=-Wall -std=c99 -I.

all: main
main: 
	gcc -o test main.c $(CFLAGS)
	gcc -o overCPU overCPU.c $(CFLAGS)
	gcc -o overMEM overMEM.c $(CFLAGS)

clean:
	rm -f *.o test overCPU overMEM
	
testCPU:
	./test cpuexample 3 20
testMEM:
	./test memexample 3 2


