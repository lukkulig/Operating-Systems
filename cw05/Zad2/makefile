CFLAGS=-Wall -std=c99 -I. -D_POSIX_C_SOURCE=199309L

all: main master slave test clean
main: 
	gcc -o main main.c $(CFLAGS)
master:
	gcc -o master master.c $(CFLAGS)
slave:
	gcc -o slave slave.c $(CFLAGS)
clean:
	rm -f *.o main master slave

test:
	./main FIFO 10 2


