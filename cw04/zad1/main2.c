#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>

bool waitFlag = false;
pid_t child;

void printTime(){
    time_t timer;
    char buffer[26];
    time(&timer);   
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&timer));
    puts(buffer);
}

void stopSignal(int signum){
	if(!waitFlag){
		kill(child,SIGKILL);
    	printf("\nOczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    }else{
    	child = fork();
		if(child == 0){
			execl("./date.sh", "./date.sh", NULL);
			printf("Something went wrong!");
			exit(EXIT_FAILURE);
		}
    	printf("\n");
    }
    
    waitFlag = !waitFlag;
    
}

void initSignal(int signum){
	printf("\nOdebrano sygnał SIGINT\n");
	kill(child,SIGKILL);
	exit(EXIT_SUCCESS);    
}

int main(void){

	struct sigaction sigact;
    sigact.sa_handler = stopSignal;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags=0;
    
    sigaction(SIGTSTP,&sigact,NULL);
    signal(SIGINT, initSignal);
    
    child = fork();
    if(child == 0){
    	execl("./date.sh", "./date.sh", NULL);
    	printf("Cannot execute ./date.sh");
    	exit(EXIT_FAILURE);
    }
    
    while(true);
    
	return 0;
}
