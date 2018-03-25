CFLAGS=-Wall -std=c99 -I.
LIB=blocksarray


all: static shared clean
static: 
	gcc -c $(LIB).c -o $(LIB).o $(CFLAGS)
	ar rcs lib$(LIB)static.a $(LIB).o 

shared: 
	gcc -fPIC -c $(LIB).c -o $(LIB).o $(CFLAGS)
	gcc -fPIC -shared -o lib$(LIB)shared.so $(LIB).o $(CFLAGS)

clean:
	rm -f *.o
