#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>

#define MAX_ARGUMENTS_AMOUNT 20

int main(int argc, char **args){

	if(argc!=4){
		printf("Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
	}
	
	FILE* file = fopen(args[1],"r");
	if(file==NULL){
		perror(args[1]);
		exit(EXIT_FAILURE);
	}
	
	rlim_t cpuTime = atoi(args[2]);
	rlim_t memSize = atoi(args[3]);
	
	char* line = NULL;
	size_t length = 0;
	char* params[MAX_ARGUMENTS_AMOUNT];
	int status;
	int lineCount=0;
	while(getline(&line,&length,file)!=-1){
		lineCount++;
		
		int i=0;
		params[i] = strtok(line," \n\t");
		while(params[i]){
			i++;
			params[i] = strtok(NULL," \n\t");			
			if(i>=MAX_ARGUMENTS_AMOUNT){
				printf("Too much arguments in command \"%s\" in line %d\n",params[0],lineCount);
				exit(EXIT_FAILURE);
			}
		}
		
		struct rusage startUsage;
		getrusage(RUSAGE_CHILDREN, &startUsage);
	
		pid_t child_pid = fork();
		if(child_pid==0){
			struct rlimit cpuLimit = {cpuTime,cpuTime};
			struct rlimit memLimit = {memSize,memSize};
			
			if(setrlimit(RLIMIT_CPU,&cpuLimit)!=0 || setrlimit(RLIMIT_MEMLOCK,&memLimit)!=0){
				printf("Cannot set limits!\n");
				exit(EXIT_FAILURE);
			}
			
			execvp(params[0],params);
		}
		
		wait(&status);
		if(status!=0){
			printf("Command \"%s\" in line %d reached resource usage limit!\n",params[0],lineCount);
			exit(EXIT_FAILURE);
		}
		
		struct rusage endUsage;
		getrusage(RUSAGE_CHILDREN,&endUsage);
		
		printf("\nFor %s: \nUser time: %d.%d sec \nSystem time: %d.%d sec\n%s\n", params[0],
  			(int)(endUsage.ru_utime.tv_sec-startUsage.ru_utime.tv_sec),
          	(int)(endUsage.ru_utime.tv_usec-startUsage.ru_utime.tv_usec),
  			(int)(endUsage.ru_stime.tv_sec-startUsage.ru_stime.tv_sec),
			(int)(endUsage.ru_stime.tv_usec-startUsage.ru_stime.tv_usec),
			"-----------------------------------");
	
	}
	fclose(file);
	return 0;
}
