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

mqd_t pubID;
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

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MSG_SIZE;

    pubID = mq_open(SERVER_PATH, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    if(pubID == -1) {
    	printf("Cannot create public queue!\n");
    	exit(EXIT_FAILURE);
    }

    Msg buff;
    while(1){
        if(!active){
        	struct mq_attr currentState;
            if(mq_getattr(pubID, &currentState) == -1){
            	printf("Cannot get current state of public queue!\n");
            	exit(EXIT_FAILURE);
            }
            if(currentState.mq_curmsgs == 0) break;
        }

        if(mq_receive(pubID, (char*)&buff, MSG_SIZE, NULL) < 0){
        	printf("Cannot receive message!\n");
        	exit(EXIT_FAILURE);
        }
        receiveMsg(&buff);
    }
    return 0;
}

void deleteQueue(){
	for(int i=0; i<MAX_CLIENTS; i++){
		if(isTakenClient[i])
			mq_close(clients[i][QID]);
	}
    mq_close(pubID);
    if(!mq_unlink(SERVER_PATH))
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

    char clientPath[15];
    sprintf(clientPath,"/%d", msg->senderPID);
    
    mqd_t clientQID = mq_open(clientPath, O_WRONLY);
    if(clientQID == -1){
    	printf("Cannot open clients queue!\n");
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

    if(mq_send(clientQID, (char*)msg, MSG_SIZE, 1) == -1){
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

    if(mq_send(clientQID, (char*)msg, MSG_SIZE, 1) == -1){
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

    if(mq_send(clientQID, (char*)msg, MSG_SIZE, 1) == -1){
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

    if(mq_send(clientQID, (char*)msg, MSG_SIZE, 1) == -1){
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
    mq_close(clientQID);
    printf("Closed client's queue. \n");
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
