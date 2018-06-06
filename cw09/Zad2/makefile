CC = gcc
CFLAGS = -std=gnu99 -Wall -lpthread
IN_FILE = prop.txt


all:
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -f *.o main
	
test:
	./main $(IN_FILE)

	
	
