#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "sleeping_barber.h"

int sem_ID=0;
int shm_ID=0;
int fifo=0;

void __exit(void);
void handleSIGTERM(int);
void initBarberShop(int seats);
void inviteClient();
void shaveClient();

int main(int argc, char** argv) {
    if(argc != 2){
    	printf("Wrong number of arguments!\n");
    	exit(EXIT_FAILURE);
    }
    int seats = (int) strtol(argv[1], 0, 10);
    if(seats<=0 || seats>MAX_CLIENTS){
    	printf("Invalid argument!\n");
    	exit(EXIT_FAILURE);
    }

    signal(SIGTERM, handleSIGTERM);
	signal(SIGINT, handleSIGTERM);
	atexit(__exit);
	initBarberShop(seats);

	while(1){
        getSem(sem_ID);
        switch(BarberShop->status){
            case BARBER_IDLE:
                if(!isQueueEmpty()){
                    inviteClient();
                    BarberShop->status = BARBER_READY;
                }else{
                    printf("%lo Barber: fell asleep\n", getTime());
                    BarberShop->status = BARBER_SLEEPING;
                }
                break;
            case BARBER_AWAKEN:
                printf("%lo Barber: woke up\n", getTime());
                BarberShop->status = BARBER_READY;
                break;
            case BARBER_SHAVING:
                shaveClient();
                BarberShop->status = BARBER_READY;
                break;
            default:
                break;
        }
        releaseSem(sem_ID);
	}

    exit(EXIT_FAILURE);
    return 0;
}

void __exit(void)
{
    if (fifo) close(fifo);
    unlink(FIFO_PATH);
    if(!shm_ID) shmctl(shm_ID, IPC_RMID, NULL);
    if(!sem_ID)semctl(sem_ID, 0, IPC_RMID, NULL);
}

void handleSIGTERM(int sig){
	printf("Closing BarberShop\n");
    exit(EXIT_SUCCESS);
}

void initBarberShop(int seats){

	if(mkfifo(FIFO_PATH, S_IRWXU | S_IRWXG | S_IRWXO)<0){
        if(errno != EEXIST){
        	perror("Barber cannot make FIFO");
        	exit(EXIT_FAILURE);
        }
    }
	if((fifo = open(FIFO_PATH, O_RDONLY | O_NONBLOCK)) < 0){
		perror("Barber cannot open FIFO");
        exit(EXIT_FAILURE);
	}

	key_t sbKey = ftok(SB_PATH, SB_ID);
    if (sbKey == -1)
		exit(EXIT_FAILURE);

	shm_ID = shmget(sbKey, sizeof(BarberShop), S_IRWXU | S_IRWXG| IPC_CREAT);
	if(shm_ID<0){
		perror("shmget");
        exit(EXIT_FAILURE);
	}
	BarberShop = shmat(shm_ID, NULL, 0);
    if(BarberShop == (void *)-1){
    	perror("shmat");
        exit(EXIT_FAILURE);
    }

    BarberShop->status=BARBER_SLEEPING;
    BarberShop->waiting_seats=seats;
	BarberShop->waiting_clients=0;
	BarberShop->current_client=-1;

	sem_ID = semget(sbKey, 1, IPC_CREAT | S_IRWXU | S_IRWXG);
    if (sem_ID== -1){
    	perror("semget");
        exit(EXIT_FAILURE);
    }
    if(semctl(sem_ID,0,SETVAL,0)<0){
    	perror("semctl");
        exit(EXIT_FAILURE);
    }
}

void inviteClient(){
    char buf[10];
    read(fifo, &buf, sizeof(char)*10);
    BarberShop->current_client=(pid_t)strtol(buf, 0, 10);
    printf("%lo Barber: invited client %d\n", getTime(), BarberShop->current_client);
}

void shaveClient(){
    pid_t client = BarberShop->current_client;
    printf("%lo Barber: started shaving client %i\n",getTime(),client);
    printf("%lo Barber: finished shaving client %i\n",getTime(),client);
    BarberShop->current_client = -1;
}
