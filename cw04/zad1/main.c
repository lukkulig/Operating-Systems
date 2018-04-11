#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

bool waitFlag = false;

void printTime(){
    time_t timer;
    char buffer[26];
    time(&timer);   
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&timer));
    puts(buffer);
}

void stopSignal(int signum){
	if(!waitFlag)
    	printf("\nOczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    else
    	printf("\n");
    waitFlag = !waitFlag;
    
}

void initSignal(int signum){
	//if(waitFlag){
		printf("\nOdebrano sygnał SIGINT\n");
		exit(EXIT_SUCCESS);
	/*}
	else
    	printf("\n");
   	*/
}

int main(void){

	struct sigaction sigact;
    sigact.sa_handler = stopSignal;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags=0;
    
    while(true){
    	
    	sigaction(SIGTSTP,&sigact,NULL);
    	signal(SIGINT, initSignal);
    	if(!waitFlag)
        	printTime();
        sleep(1);
    }
    
	return 0;
}
