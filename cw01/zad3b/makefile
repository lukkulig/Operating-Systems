CFLAGS=-Wall -std=c99 -I.
LIBSTATIC=blocksarraystatic
LIBSHARED=blocksarrayshared
LIBPATH=./../zad1
MAINPROGPATH=./../zad2

all: O0 O2 Os tests clean
static: 
	gcc -o staticTest$(OPT) $(MAINPROGPATH)/main.c -L$(LIBPATH) -l$(LIBSTATIC) $(CFLAGS) $(OPT)

shared:
	gcc -o sharedTest$(OPT) $(MAINPROGPATH)/main.c -Wl,-rpath=$(LIBPATH) -L$(LIBPATH) -l$(LIBSHARED) $(CFLAGS) $(OPT)
dynamic:
	gcc -o dynamicTest$(OPT) $(MAINPROGPATH)/main.c $(CFLAGS) -D DLL -ldl $(OPT)
	
O0:	
	make static OPT=-O0
	make shared OPT=-O0
	make dynamic OPT=-O0

O2:	
	make static OPT=-O2	
	make shared OPT=-O2
	make dynamic OPT=-O2

Os: 
	make static OPT=-Os	
	make shared OPT=-Os
	make dynamic OPT=-Os
	
	
clean:
	rm -f *.o staticTest-O0 staticTest-O2 staticTest-Os sharedTest-O0 sharedTest-O2 sharedTest-Os dynamicTest-O0 dynamicTest-O2 dynamicTest-Os

	
tests:
	echo > raport3b.txt
	echo "Static lib: " >> raport3b.txt
	echo "O0: " >> raport3b.txt
	./staticTest-O0 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000 >> raport3b.txt
	echo "O2: " >> raport3b.txt
	./staticTest-O2 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3b.txt
	echo "Os: " >> raport3b.txt
	./staticTest-Os 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3b.txt
	echo "----------------------------------------------" >> raport3b.txt
	echo "Shared lib: " >> raport3b.txt
	echo "O0: " >> raport3b.txt
	./sharedTest-O0 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000 >> raport3b.txt
	echo "O2: " >> raport3b.txt
	./sharedTest-O2 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3b.txt
	echo "Os: " >> raport3b.txt
	./sharedTest-Os 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3b.txt
	echo "----------------------------------------------" >> raport3b.txt
	echo "Dynamic lib: " >> raport3b.txt
	echo "O0: " >> raport3b.txt
	./dynamicTest-O0 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000 >> raport3b.txt
	echo "O2: " >> raport3b.txt
	./dynamicTest-O2 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3b.txt
	echo "Os: " >> raport3b.txt
	./dynamicTest-Os 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3b.txt
	echo "----------------------------------------------" >> raport3b.txt
	
