CC = gcc
CFLAGS = -std=gnu99 -Wall 

main:
	$(CC) $(CFLAGS) -o client client.c -lrt
	$(CC) $(CFLAGS) -o server server.c -lrt

clean:
	rm -f *.o client server
