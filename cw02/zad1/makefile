CFLAGS=-Wall -std=c99 -I.
LIB=blocksarray


all: main test4 test512 test4096 test8192 clean
main: 
	gcc -o test main.c $(CFLAGS)
	
clean:
	rm -f *.o test
	
fresh: 
	rm -f plik1.txt plik2.txt plik1_copy_lib.txt plik1_copy_sys.txt
test4:
	echo "" > result4.txt
	./test generate plik1.txt 5000 4 
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 5000; record length: 4" >> result4.txt
	./test sort plik1.txt  5000 4 lib >> result4.txt
	./test sort plik2.txt  5000 4 sys >> result4.txt
	echo "Copying  - num of records: 5000; record length: 4" >> result4.txt
	./test copy plik1.txt  plik1_copy_lib.txt  5000 4 lib >> result4.txt
	./test copy plik1.txt  plik1_copy_sys.txt 5000 4 sys >> result4.txt
	echo "---------------------------------------------------" >> result4.txt
	make fresh
	./test generate plik1.txt 10000 4 
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 10000; record length: 4" >> result4.txt
	./test sort plik1.txt  10000 4 lib >> result4.txt
	./test sort plik2.txt  10000 4 sys >> result4.txt
	echo "Copying  - num of records: 10000; record length: 4" >> result4.txt
	./test copy plik1.txt  plik1_copy_lib.txt  10000 4 lib >> result4.txt
	./test copy plik1.txt  plik1_copy_sys.txt 10000 4 sys >> result4.txt
	echo "---------------------------------------------------" >> result4.txt
	make fresh
test512:
	echo "" > result512.txt
	./test generate plik1.txt 5000 512 
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 5000; record length: 512" >> result512.txt
	./test sort plik1.txt  5000 512 lib >> result512.txt
	./test sort plik2.txt  5000 512 sys >> result512.txt
	echo "Copying  - num of records: 5000; record length: 512" >> result512.txt
	./test copy plik1.txt  plik1_copy_lib.txt  5000 512 lib >> result512.txt
	./test copy plik1.txt  plik1_copy_sys.txt 5000 512 sys >> result512.txt
	echo "---------------------------------------------------" >> result512.txt
	make fresh
	./test generate plik1.txt 10000 512 
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 10000; record length: 512" >> result512.txt
	./test sort plik1.txt  10000 512 lib >> result512.txt
	./test sort plik2.txt  10000 512 sys >> result512.txt
	echo "Copying  - num of records: 10000; record length: 512" >> result512.txt
	./test copy plik1.txt  plik1_copy_lib.txt  10000 512 lib >> result512.txt
	./test copy plik1.txt  plik1_copy_sys.txt 10000 512 sys >> result512.txt
	echo "---------------------------------------------------" >> result512.txt
	make fresh
test4096:	
	echo "" > result4096.txt
	./test generate plik1.txt 5000 4096
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 5000; record length: 4096" >> result4096.txt
	./test sort plik1.txt  5000 4096 lib >> result4096.txt
	./test sort plik2.txt  5000 4096 sys >> result4096.txt
	echo "Copying  - num of records: 5000; record length: 4096" >> result4096.txt
	./test copy plik1.txt  plik1_copy_lib.txt  5000 4096 lib >> result4096.txt
	./test copy plik1.txt  plik1_copy_sys.txt 5000 4096 sys >> result4096.txt
	echo "---------------------------------------------------" >> result4096.txt
	make fresh	
	./test generate plik1.txt 10000 4096
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 10000; record length: 4096" >> result4096.txt
	./test sort plik1.txt  10000 4096 lib >> result4096.txt
	./test sort plik2.txt  10000 4096 sys >> result4096.txt
	echo "Copying  - num of records: 10000; record length: 4096" >> result4096.txt
	./test copy plik1.txt  plik1_copy_lib.txt  10000 4096 lib >> result4096.txt
	./test copy plik1.txt  plik1_copy_sys.txt 10000 4096 sys >> result4096.txt
	echo "---------------------------------------------------" >> result4096.txt
	make fresh
test8192:	
	echo "" > result8192.txt
	./test generate plik1.txt 5000 8192
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 5000; record length: 8192" >> result8192.txt
	./test sort plik1.txt  5000 8192 lib >> result8192.txt
	./test sort plik2.txt  5000 8192 sys >> result8192.txt
	echo "Copying  - num of records: 5000; record length: 8192" >> result8192.txt
	./test copy plik1.txt  plik1_copy_lib.txt  5000 8192 lib >> result8192.txt
	./test copy plik1.txt  plik1_copy_sys.txt 5000 8192 sys >> result8192.txt
	echo "---------------------------------------------------" >> result8192.txt
	make fresh
	./test generate plik1.txt 10000 8192 
	cp plik1.txt plik2.txt
	echo "Sorting - num of records: 10000; record length: 8192" >> result8192.txt
	./test sort plik1.txt  10000 8192 lib >> result8192.txt
	./test sort plik2.txt  10000 8192 sys >> result8192.txt
	echo "Copying  - num of records: 10000; record length: 8192" >> result8192.txt
	./test copy plik1.txt  plik1_copy_lib.txt  10000 8192 lib >> result8192.txt
	./test copy plik1.txt  plik1_copy_sys.txt 10000 8192 sys >> result8192.txt
	echo "---------------------------------------------------" >> result8192.txt
	make fresh
	
