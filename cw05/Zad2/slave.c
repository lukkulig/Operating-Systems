#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_BUFFER_LENGTH 256

int main(int argc, char **argv){
	pid_t pid=getpid();
	srand((unsigned int)(time(NULL)*pid));
	
	if(argc<3){
		printf("Master: Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
	}
	
	int fifo = open(argv[1],O_WRONLY);
	if(fifo<0){
        printf("Slave: Cannot open FIFO %s!\n",argv[1]);
        exit(EXIT_FAILURE);
	}
	
	int N = (int)strtol(argv[2], NULL, 10);

	printf("Slave PID: %d\n", pid);

	char buffer1[MAX_BUFFER_LENGTH];
	char buffer2[MAX_BUFFER_LENGTH];
	for(int i=0; i<N; i++) {
        FILE* date = popen("date", "r");
        fgets(buffer1, MAX_BUFFER_LENGTH, date);
        sprintf(buffer2, "Slave: %d - %s", pid, buffer1);
        write(fifo, buffer2, strlen(buffer2));
        fclose(date);
        sleep((unsigned int)(rand()%4+2));
    }

	close(fifo);
	
	return 0;
}
