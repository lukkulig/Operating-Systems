#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv){

	if(argc<4){
		printf("Main: Wrong number of arguments! Using ./main fifo slaveNo N\n");
		exit(EXIT_FAILURE);
	}
	
	int N = (int) strtol(argv[2], '\0', 10);
	
	pid_t masterPID;
	
    if((masterPID = fork())<0){
    	printf("Cannot fork Master process!\n");
    	exit(EXIT_FAILURE);
    }
    
    if(!masterPID){
        execlp("./master", "./master", argv[1], NULL);
        printf("Cannot execute Master!\n");
        exit(EXIT_FAILURE);
	}
	
	sleep(1);
	
	for(int i=0; i<N; i++){
        pid_t slavePID;
        if ((slavePID = fork())< 0){
        	printf("Cannot fork Slave process!\n");
    		exit(EXIT_FAILURE);
        }
        if(!slavePID){
            execlp("./slave", "./slave", argv[1], argv[3], NULL);
            printf("Cannot execute Slave!\n");
        exit(EXIT_FAILURE);
        }
	}	

	while(wait(NULL))
		if(errno==ECHILD)
            break;
	
	return 0;
}
