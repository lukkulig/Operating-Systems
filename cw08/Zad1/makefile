CC = gcc
CFLAGS = -std=gnu99 -Wall -lpthread
IN_FILE = mountain.ascii.pgm
OUT_FILE3 = result3.pgm
OUT_FILE31 = result31.pgm
OUT_FILE65 = result65.pgm
FILTER3 = ./filters/filter3
FILTER31 = ./filters/filter31
FILTER65 = ./filters/filter65

all:
	$(CC) $(CFLAGS) -o main main.c -lm

clean:
	rm -f *.o main $(OUT_FILE3) $(OUT_FILE31) $(OUT_FILE65)
	
test:
	./main 1 $(IN_FILE) $(FILTER3) $(OUT_FILE3) > Times.txt
	./main 2 $(IN_FILE) $(FILTER3) $(OUT_FILE3) >> Times.txt
	./main 4 $(IN_FILE) $(FILTER3) $(OUT_FILE3) >> Times.txt
	./main 8 $(IN_FILE) $(FILTER3) $(OUT_FILE3) >> Times.txt
	
	./main 1 $(IN_FILE) $(FILTER31) $(OUT_FILE31) >> Times.txt
	./main 2 $(IN_FILE) $(FILTER31) $(OUT_FILE31) >> Times.txt
	./main 4 $(IN_FILE) $(FILTER31) $(OUT_FILE31) >> Times.txt
	./main 8 $(IN_FILE) $(FILTER31) $(OUT_FILE31) >> Times.txt
	
	./main 1 $(IN_FILE) $(FILTER65) $(OUT_FILE65) >> Times.txt
	./main 2 $(IN_FILE) $(FILTER65) $(OUT_FILE65) >> Times.txt
	./main 4 $(IN_FILE) $(FILTER65) $(OUT_FILE65) >> Times.txt
	./main 8 $(IN_FILE) $(FILTER65) $(OUT_FILE65) >> Times.txt
	
	
