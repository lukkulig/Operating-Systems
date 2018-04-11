#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void childHandler(int signum){
	kill(getppid(), SIGRTMIN+(rand()%(SIGRTMAX-SIGRTMIN)));
}



int main(void){

	signal(SIGUSR1,childHandler);
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set,SIGUSR1);
	
	srand((unsigned int) getpid());
	unsigned int time = rand()%11;
	
	sleep(time);
	
	kill(getppid(), SIGUSR1);
	
	fflush(stdout);
	return time;
}
