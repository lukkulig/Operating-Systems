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

#define BARBER_SLEEPING     0
#define BARBER_AWAKENING	1
#define BARBER_WAITING      2
#define BARBER_INACTIVE	    3
#define BARBER_SHAVING	    4


struct BarberShop{
	int barber_activity;
	int waiting_clients;
	int waiting_seats;
	pid_t current_client;
} *BarberShop;

long getTime(){
    struct timespec timer;
    clock_gettime(CLOCK_MONOTONIC, &timer);
    return timer.tv_nsec/1000;
}

void acquireSem(int sem_ID) {
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_op = -1;
    sem.sem_flg = 0;

    if(semop(sem_ID, &sem, 1)==-1){
    	printf("Cannot acquire semaphore!\n");                       \
		exit(EXIT_FAILURE);
    }
}

void releaseSem(int sem_ID) {
    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_op = 1;
    sem.sem_flg = 0;

    if(semop(sem_ID, &sem, 1)==-1){
    	printf("Cannot release semaphore!\n");                       \
		exit(EXIT_FAILURE);
    }
}

#endif
