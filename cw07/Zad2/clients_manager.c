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

sem_t* sem;
int shm=0;
int fifo=0;

void __exit(void);
void initClients();
void client(int shaves);
void takePlace();
void getCut();

int main(int argc, char** argv) {
    if(argc != 3){
    	printf("Wrong number of arguments!\n");
    	exit(EXIT_FAILURE);
    }
    int clients = (int) strtol(argv[1], 0, 10);
    int shaves = (int) strtol(argv[2], 0, 10);
    if(clients<=0 || shaves<=0){
    	printf("Invalid argument!\n");
    	exit(EXIT_FAILURE);
    }

	atexit(__exit);
	initClients();
    int i;
    for(i=0; i<clients; i++){
        pid_t child=fork();
        if(child < 0){
            perror("Fork");
            exit(EXIT_FAILURE);
        }
        if(child == 0)
            client(shaves);
    }
    while(wait(NULL))
        if(errno != ECHILD)
            break;
    return 0;
}

void __exit(void)
{
    if (fifo) close(fifo);
    munmap(BarberShop,sizeof(BarberShop));
    sem_close(sem);
}

void client(int shaves){
    pid_t clientPID = getpid();

    while(shaves){
        int has_took_seat=0,is_shaved=0;
        acquireSem(sem);
        //jesli golibroda spi
        if(BarberShop->barber_activity==BARBER_SLEEPING){
            printf("Client %d: woke up barber (%ld)\n", clientPID, getTime());
            BarberShop->barber_activity = BARBER_AWAKENING;

            do{
                releaseSem(sem);
                acquireSem(sem);
            }while(BarberShop->barber_activity != BARBER_WAITING);

            has_took_seat=1;
            BarberShop->barber_activity=BARBER_SHAVING;
            BarberShop->current_client = clientPID;
            printf("Client %d: took a seat (%ld)\n", clientPID, getTime());
        //jesli golibroda nie spi - znaczy, ze goli
        }else{
            //jesli w kolejce sa miejsca
            if(BarberShop->waiting_clients < BarberShop->waiting_seats){
                char buf[10];
                sprintf(buf,"%d",clientPID);
                write(fifo,&buf,sizeof(char)*10);
                BarberShop->waiting_clients += 1;
                printf("Client %d: came in the queue(%ld)\n", clientPID, getTime());
            //jesli w kolejce nie ma miejsc
            }else{
                printf("Client %d: left barber shop, because of full queue (%ld)\n", clientPID, getTime());
                releaseSem(sem);
                continue;
            }
        }
        releaseSem(sem);

        //oczekuje na zaproszenie na fotel
        while(!has_took_seat){
            acquireSem(sem);
            if(clientPID == BarberShop->current_client){
                BarberShop->barber_activity=BARBER_SHAVING;
                BarberShop->waiting_clients-=1;
                has_took_seat = 1;
                printf("Client %d: took a seat (%ld)\n", clientPID, getTime());
            }
            releaseSem(sem);
        }

        //jest strzyzony
        while(!is_shaved){
            acquireSem(sem);
            if(clientPID != BarberShop->current_client){
                BarberShop->barber_activity=BARBER_INACTIVE;
                is_shaved = 1;
                printf("Client %d: shaved and left barber shop (%ld)\n", clientPID, getTime());
                shaves--;
            }
            releaseSem(sem);
        }
    }
    _exit(EXIT_SUCCESS);
}

void initClients(){

	if((fifo = open(FIFO_PATH, O_WRONLY)) < 0){
		perror("Clients Manager cannot open FIFO");
        exit(EXIT_FAILURE);
	}


	shm = shm_open(SB_PATH, O_RDWR, S_IRWXU | S_IRWXG);
	if(shm<0){
		perror("shm_open");
        exit(EXIT_FAILURE);
	}
	if(ftruncate(shm, sizeof(BarberShop))){
        perror("ftruncate");
        exit(EXIT_FAILURE);
	}
	BarberShop = mmap(NULL, sizeof(BarberShop),PROT_READ | PROT_WRITE,MAP_SHARED,shm,0);
    if(BarberShop == (void *)-1){
    	perror("mmap");
        exit(EXIT_FAILURE);
    }

	sem = sem_open(SB_PATH, O_WRONLY, S_IRWXU | S_IRWXG,0);
    if (sem == SEM_FAILED ){
    	perror("sem_open");
        exit(EXIT_FAILURE);
    }
}

