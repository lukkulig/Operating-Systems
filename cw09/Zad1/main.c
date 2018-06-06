#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>


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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t emptyCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t fullCond = PTHREAD_COND_INITIALIZER;

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
        pthread_mutex_lock(&mutex);
        if (count == 0) break;
        pthread_mutex_unlock(&mutex);
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
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&fullCond);
    pthread_cond_destroy(&emptyCond);
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
        pthread_mutex_lock(&mutex);
        while(count == 0)
            pthread_cond_wait(&emptyCond, &mutex);

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

        if(count == buffSize-1) pthread_cond_broadcast(&fullCond);
        pthread_mutex_unlock(&mutex);
    }
}
void *prdcrRoutine(void *arg){
    char* buff = NULL;
    size_t n = 0;
    while (1)
    {
        pthread_mutex_lock(&mutex);

        while(count >= buffSize)
            pthread_cond_wait(&fullCond, &mutex);

        if (getline(&buff, &n, inFile) <= 0)
        {
            pthread_mutex_unlock(&mutex);
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

        if(count == 1) pthread_cond_broadcast(&emptyCond);
        pthread_mutex_unlock(&mutex);

        n = 0;
        buff = NULL;
    }
    pthread_exit((void*) 0);
}

