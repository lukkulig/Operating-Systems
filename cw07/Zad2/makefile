CC = gcc
CFLAGS = -std=gnu99 -Wall 

all:
	$(CC) $(CFLAGS) -o clients_manager clients_manager.c -lrt -lpthread
	$(CC) $(CFLAGS) -o barber barber.c -lrt -lpthread
clean:
	rm -f *.o clients_manager barber
