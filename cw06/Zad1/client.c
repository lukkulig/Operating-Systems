#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#include "header.h"

int privID;
int pubID;
int clientID=-1;

void deleteQueue();
void loginClient(int privKey);
void rqMirror(Msg* msg);
void rqCalc(Msg* msg);
void rqTime(Msg* msg);
void rqEnd(Msg* msg);
void rqStop(Msg* msg);


void intHandler(int signo){
    exit(2);
}

int main(void){
    if(atexit(deleteQueue) == -1){
    	printf("Cannot register client's atexit!\n");
    	exit(EXIT_FAILURE);
    }
    if(signal(SIGINT, intHandler) == SIG_ERR){
    	printf("Cannot register INT!\n");
    	exit(EXIT_FAILURE);
	}
	
    char* path = getenv("HOME");
    if(path == NULL){
    	printf("Cannot get enviromental variable 'HOME' failed!\n");
    	exit(EXIT_FAILURE);
    }
    
    

    key_t privKey = ftok(path, getpid());
    if(privKey == -1){
    	printf("Cannot generate private key!\n");
    	exit(EXIT_FAILURE);
    }

    privID = msgget(privKey, IPC_CREAT | IPC_EXCL | 0666);
    if(privID == -1){
    	printf("Cannot create private queue!\n");
    	exit(EXIT_FAILURE);
    }

    key_t publicKey = ftok(path, SEED);
    if(publicKey == -1){
    	printf("Cannot generate public key!\n");
    	exit(EXIT_FAILURE);
    }
    
	pubID = msgget(publicKey,0);
    if(pubID == -1){
    	printf("Cannot open public queue!\n");
    	exit(EXIT_FAILURE);
    }

    
    loginClient(privKey);
    
    Msg msg;
    char command[20];    
    while(1){
            
        msg.senderPID = getpid();
        printf("Request (MIRROR/CALC/TIME/END): ");
        if(fgets(command, 20, stdin) == NULL){
            printf("Cannot read command!\n");
            continue;
        }
        int n = strlen(command);
        if(command[n-1] == '\n') command[n-1] = 0;

        if(strcmp(command, "MIRROR") == 0){
            rqMirror(&msg);
        }else if(strcmp(command, "CALC") == 0){
            rqCalc(&msg);
        }else if(strcmp(command, "TIME") == 0){
            rqTime(&msg);
        }else if(strcmp(command, "END") == 0){
            rqEnd(&msg);
        }else if(strcmp(command, "q") == 0){
            exit(0);
        }else printf("Wrong command!\n");
    }
    return 0;
}

void deleteQueue(){
    msgctl(privID, IPC_RMID, NULL);
    printf("Deleted client's private queue. \n");
    Msg msg;
    msg.senderPID = getpid();
    rqStop(&msg);
}

void loginClient(int privKey){
    Msg msg;
    msg.msgType = LOGIN;
    msg.senderPID = getpid();
    sprintf(msg.txt, "%d", privKey);

    if(msgsnd(pubID, &msg, MSG_SIZE, 0) == -1){
    	printf("LOGIN request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(msgrcv(privID, &msg, MSG_SIZE, 0, 0) == -1){
    	printf("Cannot get LOGIN response!\n");
   		exit(EXIT_FAILURE);
    }
    if(sscanf(msg.txt, "%d", &clientID) < 1){
    	printf("Cannot scan LOGIN response!\n");
    	exit(EXIT_FAILURE);
    }
    if(clientID < 0){
    	printf("Cannot add more clients to server!\n");
    	exit(EXIT_FAILURE);
    }
    
    printf("Client logged in! ClientID: %d!\n", clientID);
}

void rqMirror(Msg* msg){
    
    msg->msgType = MIRROR;
    printf("Expression to mirror: ");
    if(fgets(msg->txt, MAX_MSG_LEN, stdin) == NULL){
        printf("Too long expression!\n");
        return;
    }
    
    if(msgsnd(pubID, msg, MSG_SIZE, 0) == -1){
    	printf("MIRROR request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(msgrcv(privID, msg, MSG_SIZE, 0, 0) == -1){
    	printf("Cannot get MIRROR response!\n");
    	exit(EXIT_FAILURE);
    }
    printf("%s", msg->txt);
}

void rqCalc(Msg* msg){
    msg->msgType = CALC;
    printf("Expression to calculate: ");
    if(fgets(msg->txt, MAX_MSG_LEN, stdin) == NULL){
        printf("Too long expression!\n");
        return;
    }
    if(msgsnd(pubID, msg, MSG_SIZE, 0) == -1){
    	printf("CALC request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(msgrcv(privID, msg, MSG_SIZE, 0, 0) == -1){
    	printf("Cannot get CALC response!\n");
    	exit(EXIT_FAILURE);
    }
    printf("%s", msg->txt);
}

void rqEnd(Msg* msg){
    msg->msgType = END;
    if(msgsnd(pubID, msg, MSG_SIZE, 0) == -1){
    	printf("END request failed!\n");
    	exit(EXIT_FAILURE);
    }
    exit(2);
}

void rqStop(Msg* msg){
    msg->msgType = STOP;
    if(msgsnd(pubID, msg, MSG_SIZE, 0) == -1){
    	printf("STOP request failed!\n");
    	exit(EXIT_FAILURE);
    }
}

void rqTime(Msg* msg){
    msg->msgType = TIME;

    if(msgsnd(pubID, msg, MSG_SIZE, 0) == -1){
    	printf("TIME request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(msgrcv(privID, msg, MSG_SIZE, 0, 0) == -1){
    	printf("Cannot get TIME response!\n");
    	exit(EXIT_FAILURE);
    }
    printf("%s", msg->txt);
}
