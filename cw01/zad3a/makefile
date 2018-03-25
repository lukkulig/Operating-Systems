CFLAGS=-Wall -std=c99 -I.
LIBSTATIC=blocksarraystatic
LIBSHARED=blocksarrayshared
LIBPATH=./../zad1
MAINPROGPATH=./../zad2

all:  static shared dynamic tests clean
static: 
	gcc -o staticTest $(MAINPROGPATH)/main.c -L$(LIBPATH) -l$(LIBSTATIC) $(CFLAGS)

shared:
	gcc -o sharedTest $(MAINPROGPATH)/main.c -Wl,-rpath=$(LIBPATH) -L$(LIBPATH) -l$(LIBSHARED) $(CFLAGS) 
dynamic:
	gcc -o dynamicTest $(MAINPROGPATH)/main.c $(CFLAGS) -D DLL -ldl
	
clean:
	rm -f *.o staticTest sharedTest dynamicTest

tests:
	echo > raport3a.txt
	echo "Static lib: " >> raport3a.txt
	./staticTest 95000 5000 static del_and_add 5000 del_then_add 5000 find 20000 >> raport3a.txt
	./staticTest 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3a.txt
	echo "----------------------------------------------" >> raport3a.txt
	echo "Shared lib: " >> raport3a.txt
	./sharedTest 95000 5000 static del_and_add 5000 del_then_add 5000 find 20000 >> raport3a.txt
	./sharedTest 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3a.txt
	echo "----------------------------------------------" >> raport3a.txt
	echo "Dynamic lib: " >> raport3a.txt
	./dynamicTest 95000 5000 static del_and_add 5000 del_then_add 5000 find 20000 >> raport3a.txt
	./dynamicTest 95000 5000 dynamic del_and_add 5000 del_then_add 5000 find 20000  >> raport3a.txt
	echo "----------------------------------------------" >> raport3a.txt
	
