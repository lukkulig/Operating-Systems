#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#define MAX_BUFFER_LENGTH 256

int main(int argc, char **argv){

	if(argc<2){
		printf("Master: Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
	}
	

	if(mkfifo(argv[1], S_IRUSR | S_IWUSR) == -1){
        printf("Master: Cannot create FIFO %s\n",argv[1]);
        exit(EXIT_FAILURE);
	}
	
	FILE *fifo = fopen(argv[1], "r");
    if(!fifo){
        printf("Master: Cannot open FIFO %s\n",argv[1]);
        exit(EXIT_FAILURE);
	}
	
	char buffer[MAX_BUFFER_LENGTH];
	while (fgets(buffer, MAX_BUFFER_LENGTH, fifo) != NULL) {
        write(1, buffer, strlen(buffer));
    }
    
    printf("Master: Reading done!\n");
    
	fclose(fifo);
	
	if (remove(argv[1])) {
        printf("Master: Cannot delete FIFO %s\n",argv[1]);
        exit(EXIT_FAILURE);
	}
	
	return 0;
}
