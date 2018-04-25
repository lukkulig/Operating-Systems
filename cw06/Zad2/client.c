#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

#include "header.h"

mqd_t privID;
mqd_t pubID;
int clientID=-1;
char privPath[20];

void deleteQueue();
void loginClient(char* privPath);
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
	
    sprintf(privPath,"/%d",getpid());
    
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE; 

    privID = mq_open(privPath, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    if(privID == -1){
    	printf("Cannot create private queue!\n");
    	exit(EXIT_FAILURE);
    }

    
	pubID = mq_open(SERVER_PATH,O_WRONLY);
    if(pubID == -1){
    	printf("Cannot open public queue!\n");
    	exit(EXIT_FAILURE);
    }

    
    loginClient(privPath);
    
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
	if(clientID>=0){
		Msg msg;
		msg.senderPID = getpid();
    	rqStop(&msg);
    }
    mq_close(pubID);
    mq_close(privID);
    mq_unlink(privPath);
    printf("Deleted client's private queue. \n");
}

void loginClient(char* privKey){
    Msg msg;
    msg.msgType = LOGIN;
    msg.senderPID = getpid();
    sprintf(msg.txt, "%s", privPath);

    if(mq_send(pubID,(char*)&msg, MSG_SIZE, 1) == -1){
    	printf("LOGIN request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(mq_receive(privID,(char*)&msg, MSG_SIZE, NULL) == -1){
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
    
    if(mq_send(pubID, (char*)msg, MSG_SIZE, 1) == -1){
    	printf("MIRROR request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(mq_receive(privID, (char*)msg, MSG_SIZE, NULL) == -1){
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
    if(mq_send(pubID, (char*)msg, MSG_SIZE, 1) == -1){
    	printf("CALC request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(mq_receive(privID, (char*)msg, MSG_SIZE, NULL) == -1){
    	printf("Cannot get CALC response!\n");
    	exit(EXIT_FAILURE);
    }
    printf("%s", msg->txt);
}

void rqEnd(Msg* msg){
    msg->msgType = END;
    if(mq_send(pubID, (char*)msg, MSG_SIZE, 1) == -1){
    	printf("END request failed!\n");
    	exit(EXIT_FAILURE);
    }
    exit(2);
}

void rqStop(Msg* msg){
    msg->msgType = STOP;
    if(mq_send(pubID, (char*)msg, MSG_SIZE, 1) == -1){
    	printf("STOP request failed!\n");
    	exit(EXIT_FAILURE);
    }
}

void rqTime(Msg* msg){
    msg->msgType = TIME;

    if(mq_send(pubID, (char*)msg, MSG_SIZE, 1) == -1){
    	printf("TIME request failed!\n");
    	exit(EXIT_FAILURE);
    }
    if(mq_receive(privID, (char*)msg, MSG_SIZE, NULL) == -1){
    	printf("Cannot get TIME response!\n");
    	exit(EXIT_FAILURE);
    }
    printf("%s", msg->txt);
}
