#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

volatile int N;
volatile int M;
volatile int n;
volatile int m;
volatile pid_t *childrens;
volatile pid_t *awaitings;

void handlerSIGINT(int, siginfo_t *, void *);
void handlerSIGUSR(int, siginfo_t *, void *);
void handlerSIGCHLD(int, siginfo_t *, void *);
void handlerSIGRT(int, siginfo_t *, void *);


int main(int argc, char *argv[]){

	if(argc<3){
		printf("Wrong number of args! Use ./test N M\n");
		exit(EXIT_FAILURE);
    }
    
    N = (int) strtol(argv[1], '\0', 10);
    M = (int) strtol(argv[2], '\0', 10);
    
    if(N<1){
		printf("Wrong L arg!\n");
		exit(EXIT_FAILURE);
    }
    if(M<1){
		printf("Wrong M arg!\n");
		exit(EXIT_FAILURE);
    }
    if(N<M){
		printf("N smaller than M!\n");
		exit(EXIT_FAILURE);
    }
    
    childrens= calloc((size_t) N, sizeof(pid_t));
	awaitings = calloc((size_t) N, sizeof(pid_t));
    n=m=0;
    
    struct sigaction act;
    sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	
	act.sa_sigaction = handlerSIGINT;
    if (sigaction(SIGINT, &act, NULL) == -1){
    	printf("Can't catch SIGINT!\n");
    	exit(EXIT_FAILURE);
    }
    
    act.sa_sigaction = handlerSIGUSR;
    if (sigaction(SIGUSR1, &act, NULL) == -1){
    	printf("Can't catch SIGUSR1!\n");
    	exit(EXIT_FAILURE);
	}
	
    act.sa_sigaction = handlerSIGCHLD;
	if (sigaction(SIGCHLD, &act, NULL) == -1){
    	printf("Can't catch SIGCHLD!\n");
    	exit(EXIT_FAILURE);
	}
	
	for(int i=SIGRTMIN; i<=SIGRTMAX; i++){
        act.sa_sigaction = handlerSIGRT;
        if (sigaction(i, &act, NULL) == -1){
        	printf("Can't catch SIGRTMIN+%d!\n", i-SIGRTMIN);
        	exit(EXIT_FAILURE);
		}
	}
	
	for(int i = 0; i < N; i++){
        pid_t child = fork();
        if (child==0) {
            execl("./child", "./child", NULL);
            printf("Cannot create child process!\n");
            exit(EXIT_FAILURE);
        } else {
            childrens[n] = child;
            n++;
        }
	}
	
	while(wait(NULL))
		if (errno == ECHILD){
			printf("No child process\n");
			exit(EXIT_FAILURE);
		}
	
	return 0;
}
void handlerSIGINT(int signum, siginfo_t *info, void *context){
	printf("Mother received SIGINT");
	for (int i=0; i<N; i++){
        if (childrens[i] != -1) {
            kill(childrens[i], SIGKILL);
            waitpid(childrens[i], NULL, 0);
        }
    }
	exit(EXIT_SUCCESS);
}

bool isChildren(pid_t pid){
	for (int i = 0; i < N; i++)
        if (childrens[i] == pid)return true;
    return false;
}

void handlerSIGUSR(int signum, siginfo_t *info, void *context){
	printf("Mother received SIGUSR1 from PID: %d\n",info->si_pid);	
	if(!isChildren(info->si_pid)) return;
	
	if (m >= M) {
        printf("Mother sent SIGUSR1 to child: %d\n", info->si_pid);
        kill(info->si_pid, SIGUSR1);
        waitpid(info->si_pid, NULL, 0);
	}else{
		awaitings[m]=info->si_pid;
		m++;
        if(m>=M){
            for (int i=0; i<M; i++) {
                if(awaitings[i]>0){
                    printf("Mother sent SIGUSR1 to child: %d\n",awaitings[i]);
                    kill(awaitings[i], SIGUSR1);
                    waitpid(awaitings[i], NULL, 0);
                }
            }
        }
	}
    
}
void handlerSIGCHLD(int signum, siginfo_t *info, void *context){
	printf("Child %d returned: %d\n", info->si_pid, info->si_status);
	n--;
	if(n==0){
        printf("All childrens terminated!\n");
        exit(EXIT_SUCCESS);
    }
		
	for (int i=0; i<N; i++)
       if (childrens[i] == info->si_pid){
            childrens[i] = -1;
            break;
       }     
}
void handlerSIGRT(int signum, siginfo_t *info, void *context){
	printf("Mother received SIGMIN+%i from PID: %d\n", signum-SIGRTMIN,info->si_pid);
}
