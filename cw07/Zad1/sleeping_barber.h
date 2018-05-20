#ifndef SLEEPING_BARBER_H
#define SLEEPING_BARBER_H

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>

#define MAX_CLIENTS 	32
#define FIFO_PATH 		"fifo"
#define SB_ID			0777
#define SB_PATH			getenv("HOME")

#define BARBER_SLEEPING 0
#define BARBER_AWAKEN 	1
#define BARBER_READY 	2
#define BARBER_IDLE		3
#define BARBER_SHAVING	4

#define CLIENT_ARRIVED	0
#define CLINET_INVITED	1
#define CLIENT_SHAVED	2

struct BarberShop{
	int status;
	int waiting_clients;
	int waiting_seats;
	pid_t current_client;
} *BarberShop;

long getTime(){
    struct timespec timer;
    clock_gettime(CLOCK_MONOTONIC, &timer);
    return timer.tv_nsec/1000;
}

void getSem(int sem_ID) {
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_op = -1;

    if(semop(sem_ID, &sem, 1)==-1){
    	printf("Cannot execute operation on semaphore!\n");                       \
		exit(EXIT_FAILURE);
    }
}

void releaseSem(int sem_ID) {
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_op = 1;

    if(semop(sem_ID, &sem, 1)==-1){
    	printf("Cannot execute operation on semaphore!\n");                       \
		exit(EXIT_FAILURE);
    }
}


bool isQueueEmpty(){
    if(BarberShop->waiting_clients == 0) return true;
    return false;
}

bool isQueueFull(){
    if(BarberShop->waiting_clients < BarberShop->waiting_seats) return false;
    return true;
}

#endif
