#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

volatile int L;
volatile int Type;
volatile int parentSIGSent = 0;
volatile int parentSIGReceived = 0;
volatile int childSIGReceived = 0;
volatile pid_t childPID;
volatile pid_t parentPID;

void child();
void parent();

void childHandler (int signum, siginfo_t *siginfo, void *context);
void parentHandler(int signum, siginfo_t *siginfo, void *context);


int main(int argc, char *argv[]){

	if(argc<3){
		printf("Wrong number of args! Use ./test L Type\n");
		exit(EXIT_FAILURE);
    }
    
    L = (int) strtol(argv[1], '\0', 10);
    Type = (int) strtol(argv[2], '\0', 10);
    
    if(L<1){
		printf("Wrong L arg!\n");
		exit(EXIT_FAILURE);
    }
    if(Type<1 || Type>3){
		printf("Wrong Type arg!\n");
		exit(EXIT_FAILURE);
    }
    
    
    childPID=fork();
    if((childPID) == 0)
    	child();
    else if (childPID>0)
    	parent();
    else{
    	printf("Cannot fork!\n");
		exit(EXIT_FAILURE);
    }    
    
    printf("Sent: %d\t Parent received: %d \n",
    	parentSIGSent,parentSIGReceived);
    
	return 0;
}



void parent(){
	struct sigaction act;
    act.sa_handler = NULL;
    act.sa_sigaction = parentHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    if(Type==3){
        sigaddset(&act.sa_mask, SIGRTMIN);
        sigaddset(&act.sa_mask, SIGRTMIN + 1);
    }

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGCONT, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);
	sigaction(SIGRTMIN + 1, &act, NULL);
	sleep(10);
	
	sigset_t set;
	switch(Type){
        case 1:
		    for(int i = 0; i < L; i++) {
		        kill(childPID, SIGUSR1);
		        printf("Sent %d SIGUSR1 to child. \n", ++parentSIGSent);
		        sleep(1);
		    }
		    while(parentSIGSent!=parentSIGReceived)
				sleep(10);
		    kill(childPID, SIGUSR2);
			break;
        case 2:
			sigfillset(&set);
			sigdelset(&set, SIGUSR1);
			sigdelset(&set, SIGINT);

			for(int i = 0; i < L; i++) {
				printf("Sent %d SIGUSR1 to child. \n", ++parentSIGSent);
				kill(childPID, SIGUSR1);
				sigsuspend(&set);
				//pause();
            }
            while(parentSIGSent!=parentSIGReceived)
				sleep(10);
            kill(childPID, SIGUSR2);
			break;
        case 3:
		   	for(int i = 0; i < L; i++) {
				kill(childPID, SIGRTMIN);
				printf("Sent %d SIGRTMIN to child. \n", ++parentSIGSent);
		    }
		    while(parentSIGSent!=parentSIGReceived)
				sleep(10);
			kill(childPID, SIGRTMIN+1);
            break;
        default:
            exit(1);
	}	
}

void child(){
	parentPID = getppid();

	sigset_t set;
    sigfillset(&set);
    if(Type==1 || Type==2) {
        sigdelset(&set, SIGUSR1);
        sigdelset(&set, SIGUSR2);
    } else if (Type==3){
        sigdelset(&set, SIGRTMIN);
        sigdelset(&set, SIGRTMIN + 1);
    }
	sigprocmask(SIG_SETMASK, &set, NULL);

	struct sigaction act;
    act.sa_handler = NULL;
    act.sa_sigaction = childHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &act, NULL) == -1 ||
        sigaction(SIGUSR2, &act, NULL) == -1 ||
        sigaction(SIGRTMIN, &act, NULL) == -1 ||
        sigaction(SIGRTMIN + 1, &act, NULL) == -1) {
        fprintf(stderr, "Can't set child handler.");
    }

    kill(parentPID, SIGCONT);

    while (1)
		sleep(5);
}

void childHandler(int signum, siginfo_t *siginfo, void *context){
	if (signum == SIGUSR1 && (Type==1 || Type==2)) {
        kill(parentPID, SIGUSR1);
		printf("Child received %d SIGUSR1.\n", ++childSIGReceived);
    }else if (signum == SIGUSR2 && (Type==1 || Type==2)) {
        exit(0);
    }else if (signum == SIGRTMIN && Type==3) {
        kill(parentPID, SIGRTMIN);
		printf("Child received %d SIGRTMIN.\n", ++childSIGReceived);
    }else if (signum == SIGRTMIN+1 && Type==3) {
        exit(0);
	}	
}

void parentHandler(int signum, siginfo_t *siginfo, void *context){
	if (signum == SIGUSR1 || signum == SIGRTMIN || signum == SIGRTMIN + 1) {
        printf("Parent received %d SIG%d. \n", ++parentSIGReceived,signum);
    } else if (signum == SIGINT) {
        kill(childPID, SIGUSR2);
        exit(9);
    } else if (signum == SIGCONT) {
        printf("Parent received SIGCONT. Work start.\n");
    } else if (signum == SIGCHLD) {
        printf("Parent received SIGCHLD. Work end.\n");
        exit(0);
	}
}


