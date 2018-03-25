CFLAGS=-Wall -std=c99 -ggdb
LIB=blocksarraystatic
LIBPATH=./../zad1

all: zad2 clean


zad2:
	gcc -o test main.c -L$(LIBPATH) -l$(LIB) $(CFLAGS)
	make tests
	
clean:
	rm -f *.o	test

	
tests:
	echo "" > raport2.txt
	./test 95000 10000 static del_and_add 10000 del_then_add 10000 find 20000 >> raport2.txt
	./test 95000 10000 dynamic del_and_add 10000 del_then_add 10000 find 20000  >> raport2.txt
	echo "----------------------------------------------" >> raport2.txt
