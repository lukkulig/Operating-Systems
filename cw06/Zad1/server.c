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

#define PID 0
#define QID 1
#define FREE 2

void deleteQueue();
void receiveMsg(Msg* msg);
void login(Msg* msg);
void mirror(Msg* msg);
void calc(Msg* msg);
void timeEx(Msg* msg);
void end(Msg* msg);
void stop(Msg* msg);
int prepareMsg(Msg* msg);
int findQID(pid_t client_pid);
int getFreeClientID();

void intHandler(int signum){
    exit(2);    
}

int pubID;
bool active = true;
int clients[MAX_CLIENTS][2];
int isTakenClient[MAX_CLIENTS];

int main(void){
    if(atexit(deleteQueue) == -1){
    	printf("Cannot register severs's atexit!\n");
    	exit(EXIT_FAILURE);
    }
    if(signal(SIGINT, intHandler) == SIG_ERR){
    	printf("Cannot register INT!\n");
    	exit(EXIT_FAILURE);
	}

    struct msqid_ds publicQueue;
    char* path = getenv("HOME");
    if(path == NULL){
    	printf("Cannot get enviromental variable 'HOME' failed!\n");
    	exit(EXIT_FAILURE);
    }

    key_t pubKey = ftok(path, SEED);
    if(pubKey == -1){
    	printf("Cannot generate public key!\n");
    	exit(EXIT_FAILURE);
    }

    pubID = msgget(pubKey, IPC_CREAT | IPC_EXCL | 0666);
    if(pubID == -1) {
    	printf("Cannot create public queue!\n");
    	exit(EXIT_FAILURE);
    }

    Msg buff;
    while(1){
        if(!active){
            if(msgctl(pubID, IPC_STAT, &publicQueue) == -1){
            	printf("Cannot get current state of public queue!\n");
            	exit(EXIT_FAILURE);
            }
            if(publicQueue.msg_qnum == 0) break;
        }

        if(msgrcv(pubID, &buff, MSG_SIZE, 0, 0) < 0){
        	printf("Cannot receive message!\n");
        	exit(EXIT_FAILURE);
        }
        receiveMsg(&buff);
    }
    return 0;
}

void deleteQueue(){
    msgctl(pubID, IPC_RMID, NULL);
    printf("Delete public queue. \n");
}

void receiveMsg(Msg* msg){
    if(msg==NULL) return;
    switch(msg->msgType){
    	case LOGIN:
    		login(msg);
    		break;
        case MIRROR:
            mirror(msg);
            break;
        case CALC:
            calc(msg);
            break;
        case TIME:
            timeEx(msg);
            break;
        case END:
            end(msg);
            break;
        case STOP:
        	stop(msg);
        	break;
        default:
            break;
    }
}

void login(Msg* msg){

    key_t clientQKey;
    if(sscanf(msg->txt, "%d", &clientQKey) < 0){
    	printf("Cannot read client key!\n");
    	exit(EXIT_FAILURE);
    }
    
    int clientQID = msgget(clientQKey, 0);
    if(clientQID == -1){
    	printf("Cannot read client QID!\n");
    	exit(EXIT_FAILURE);
    }
    
    int clientPID = msg->senderPID;
	
	int clientID = getFreeClientID();

	if(clientID < 0){
        sscanf(msg->txt, "%d", &clientID);
	}else{
		clients[clientID][PID] = clientPID;
    	clients[clientID][QID] = clientQID;
    	msg->msgType = LOGIN;
    	msg->senderPID = getpid();
    	sprintf(msg->txt, "%d", clientID);
	}

    if(msgsnd(clientQID, msg, MSG_SIZE, 0) == -1){
    	printf("LOGIN response failed!\n");
    	exit(EXIT_FAILURE);
    }
}

void mirror(Msg* msg){
    
    int clientQID = prepareMsg(msg);
    if(clientQID==-1) return;
    
    int msgLen = (int)strlen(msg->txt);
    if(msg->txt[msgLen-1] == '\n') msgLen--;
    
    for(int i=0; i < msgLen / 2; i++){
        char buff = msg->txt[i];
        msg->txt[i] = msg->txt[msgLen - i - 1];
        msg->txt[msgLen - i - 1] = buff;
    }

    if(msgsnd(clientQID, msg, MSG_SIZE, 0) == -1){
    	printf("MIRROR response failed!\n");
    	exit(EXIT_FAILURE);
    }
}

void calc(Msg* msg){
    int clientQID = prepareMsg(msg);
    if(clientQID == -1) return;

    char cmd[MAX_MSG_LEN+10];
    sprintf(cmd, "echo '%s' | bc", msg->txt);
    FILE* calc = popen(cmd, "r");
    fgets(msg->txt, MAX_MSG_LEN, calc);
    pclose(calc);

    if(msgsnd(clientQID, msg, MSG_SIZE, 0) == -1){
    	printf("CALC response failed!\n");
    	exit(EXIT_FAILURE);
    }
}

void timeEx(Msg* msg){
    int clientQID = prepareMsg(msg);
    if(clientQID == -1) return;

    FILE* date = popen("date", "r");
    fgets(msg->txt, MAX_MSG_LEN, date);
    pclose(date);

    if(msgsnd(clientQID, msg, MSG_SIZE, 0) == -1){
    	printf("TIME response failed!\n");
    	exit(EXIT_FAILURE);
    }
}
void end(Msg* msg){
    active = false;
}

void stop(Msg* msg){
	int clientQID;
	int i=0;
    for(i = 0; i < MAX_CLIENTS; i++){
        if(clients[i][PID] == msg->senderPID){
            clientQID = clients[i][QID];
            break;       
        }
    }
    isTakenClient[i] = 0;
    msgctl(clientQID, IPC_RMID, NULL);
    printf("Delete client's queue. \n");
}

int prepareMsg(Msg* msg){
    int clientQID = findQID(msg->senderPID);
    if(clientQID == -1){
        printf("Cannot find client!\n");
        return -1;
    }
    msg->msgType = msg->senderPID;
    msg->senderPID = getpid();

    return clientQID;
}

int findQID(pid_t senderPID){
    for(int i=0; i<MAX_CLIENTS; i++)
        if(clients[i][PID] == senderPID) return clients[i][QID];
    return -1;
}



int getFreeClientID(){
	int i=0;
	while(i<MAX_CLIENTS && isTakenClient[i]) i++;
	if(i==MAX_CLIENTS) return -1;
	isTakenClient[i]=1;
	return i;
}
