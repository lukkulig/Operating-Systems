#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

int prdcrAmount; 		//P
int cnsmrAmount;		//K
int buffSize;			//N
FILE* inFile;
int	compareLen;			//L
char compareMode;
char displayMode;
int nk;

char** circBuffer;
int	currPrdcr = 0;
int currCnsmr = 0;
int count = 0;

sem_t sem;
sem_t emptySem;
sem_t fullSem;

pthread_t *prdcrs;
pthread_t *cnsmrs;

void init(char* propFilename);
void __exit();
void handlerINT(int);
void *cnsmrRoutine(void *arg);
void *prdcrRoutine(void *arg);

int main(int argc, char** argv) {
	if(argc != 2){
		printf("Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
	}

	init(argv[1]);
	atexit(__exit);

	if(nk == 0){
        signal(SIGINT, handlerINT);
	} else {
        signal(SIGALRM, handlerINT);
	}

	for(int i = 0; i < prdcrAmount; i++)
    {
        int res = pthread_create(&prdcrs[i], NULL, &prdcrRoutine, NULL);
        if(res != 0){
        	errno = res;
         	perror("pthreat_create");
         	exit(EXIT_FAILURE);
        }
	}
	for(int i = 0; i < cnsmrAmount; i++)
    {
        int res = pthread_create(&cnsmrs[i], NULL, &cnsmrRoutine, NULL);
        if(res != 0){
        	errno = res;
         	perror("pthreat_create");
         	exit(EXIT_FAILURE);
        }
	}
	printf("made threads\n");
	if(nk>0) alarm(nk);

	for(int i = 0; i < prdcrAmount; i++) pthread_join(prdcrs[i], NULL);

    while(1){
        sem_wait(&sem);
        if (count == 0) break;
        sem_post(&sem);
    }
	return 0;
}

void init(char* propFilename){
	FILE *propFile = fopen(propFilename, "r");
	if(propFile == NULL){
		perror("Cannot open properties file");
		exit(EXIT_FAILURE);
	}
	char* inFilename = calloc(30,sizeof(char));
	if((fscanf(propFile, "%d\n%d\n%d\n%s\n%d\n%c\n%c\n%d", &prdcrAmount,&cnsmrAmount,&buffSize, inFilename, &compareLen, &compareMode, &displayMode, &nk)!= 8) || prdcrAmount < 1 || cnsmrAmount < 1 || buffSize < 1 || compareLen < 1 || nk < 0){
    	printf("Wrong format of properties file!\nUsage format: P K N input_file L [> | < | =] [F | S] nk\n");
    	exit(EXIT_FAILURE);
	}

	fclose(propFile);

    inFile = fopen(inFilename, "r");
	if(inFile == NULL){
		perror("Cannot open input file");
		exit(EXIT_FAILURE);
	}

	free(inFilename);

    circBuffer = calloc(buffSize, sizeof(char*));
    prdcrs = calloc(prdcrAmount, sizeof(pthread_t));
    cnsmrs = calloc(cnsmrAmount, sizeof(pthread_t));
    sem_init(&sem, 0, 1);
    sem_init(&fullSem, 0, buffSize);
    sem_init(&emptySem, 0, 0);
}

void __exit(){
    if(circBuffer) free(circBuffer);
    if(inFile) fclose(inFile);

    for(int i = 0; i < prdcrAmount; i++)
        pthread_cancel(prdcrs[i]);

    for(int i = 0; i < cnsmrAmount; i++)
        pthread_cancel(cnsmrs[i]);

    if(prdcrs) free(prdcrs);
    if(cnsmrs) free(cnsmrs);

    sem_destroy(&sem);
    sem_destroy(&fullSem);
    sem_destroy(&emptySem);
}

void handlerINT(int sig){
    for(int i = 0; i < prdcrAmount; i++)
        pthread_cancel(prdcrs[i]);

    for(int i = 0; i < cnsmrAmount; i++)
        pthread_cancel(cnsmrs[i]);
    exit(EXIT_FAILURE);
}

void *cnsmrRoutine(void *arg){
    char* buff;
    while (1)
    {

        sem_wait(&emptySem);
        sem_wait(&sem);

        buff = circBuffer[currCnsmr];
        circBuffer[currCnsmr] = NULL;

        if (displayMode == 'F')
            printf("Consumer reads a line from %i \t%i / %i\n", currCnsmr, count-1, buffSize);


        if (buff[strlen(buff)-1] == '\n') buff[strlen(buff)-1] = '\0';
        int flag;
        switch(compareMode){
            case '=':   flag = (strlen(buff) == compareLen); break;
            case '>':   flag = (strlen(buff) > compareLen);  break;
            case '<':   flag = (strlen(buff) < compareLen);  break;
        }
        if(flag)
            printf("%i : %s\n", currCnsmr, buff);

        if(buff) free(buff);
        if(circBuffer[currCnsmr]) free(circBuffer[currCnsmr]);
        currCnsmr = (currCnsmr+1)%buffSize;
        count--;

        sem_post(&fullSem);
        sem_post(&sem);
    }
}
void *prdcrRoutine(void *arg){
    char* buff = NULL;
    size_t n = 0;
    while (1)
    {
        sem_wait(&fullSem);
        sem_wait(&sem);

        if (getline(&buff, &n, inFile) <= 0)
        {
            sem_post(&sem);
            break;
        }
        circBuffer[currPrdcr] = malloc(sizeof(char)*n);
        strcpy(circBuffer[currPrdcr],buff);
        if (displayMode == 'F'){
            printf("Producer puts a line into %i, length %li \t%i / %i\n", currPrdcr, n, count+1, buffSize);
            printf("> %s\n",circBuffer[currPrdcr]);
        }
        currPrdcr = (currPrdcr+1)%buffSize;
        count++;
        n = 0;
        buff = NULL;

        sem_post(&emptySem);
        sem_post(&sem);


    }
    pthread_exit((void*) 0);
}

