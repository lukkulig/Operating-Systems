#ifndef SLEEPING_BARBER_H
#define SLEEPING_BARBER_H


#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>

#define MAX_CLIENTS 	32
#define FIFO_PATH 		"fifo"
#define SB_ID			0777
#define SB_PATH			"/barber_shop"

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

void acquireSem(sem_t *sem) {
    if((sem_wait(sem))==-1){
        perror("Cannot acquire semaphore!\n");
        exit(EXIT_FAILURE);
    }
}

void releaseSem(sem_t *sem) {

    if((sem_post(sem))==-1){
    	printf("Cannot release semaphore!\n");                       \
		exit(EXIT_FAILURE);
    }
}

#endif
