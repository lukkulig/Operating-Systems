#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGUMENTS_AMOUNT 20

int main(int argc, char **args){

	if(argc<2){
		printf("Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
	}
	
	FILE* file = fopen(args[1],"r");
	if(file==NULL){
		perror(args[1]);
		exit(EXIT_FAILURE);
	}
	
	char* line = NULL;
	size_t length = 0;
	char* params[MAX_ARGUMENTS_AMOUNT];
	int status;
	int lineCount=0;
	while(getline(&line,&length,file)!=-1){
		lineCount++;
		
		int i=0;
		while((params[i] = strtok(i==0 ? line:NULL," \n\t"))!=NULL){
			i++;
			if(i>=MAX_ARGUMENTS_AMOUNT){
				printf("Too much arguments in command \"%s\" in line %d\n",params[0],lineCount);
				exit(EXIT_FAILURE);
			}
		}

		pid_t child_pid = fork();
		if(child_pid==0){
			execvp(params[0],params);
		}
		
		wait(&status);
		if(status==0){
			continue;
		}else{
			printf("Cannot run command \"%s\" in line %d\n",params[0],lineCount);
			exit(EXIT_FAILURE);
		}
	
	}

	fclose(file);
	return 0;
}
