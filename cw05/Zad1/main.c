#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGUMENTS_AMOUNT 6
#define MAX_COMMANDS_AMOUNT 6

void execLine(char* line, const int lineCount){
	char* commands[MAX_COMMANDS_AMOUNT];
	char* arguments[MAX_ARGUMENTS_AMOUNT];
	int commandNo=0;
	
	commands[commandNo] = strtok(line,"|");
	while(commands[commandNo]){
		commandNo++;
		commands[commandNo] = strtok(NULL,"|");			
		if(commandNo>=MAX_COMMANDS_AMOUNT){
			printf("Too much commands in line %d\n",lineCount);
			exit(EXIT_FAILURE);
		}
	}
	
	int pipes[2][2];
	int i;
	for(i=0; i<commandNo; i++){
		int argNo=0;
		arguments[argNo] = strtok(commands[i]," \t\n");
		while(arguments[argNo]){
			argNo++;
			arguments[argNo] = strtok(NULL," \t\n");			
			if(argNo>=MAX_ARGUMENTS_AMOUNT){
				printf("Too much arguments in command in line %d\n",lineCount);
				exit(EXIT_FAILURE);
			}
		}
	
	
		if(i>0){
			close(pipes[i%2][0]);
			close(pipes[i%2][1]);
		}
		
		if(pipe(pipes[i%2])==-1){
			printf("Cannot create pipe!");
			exit(EXIT_FAILURE);
		}
		
		pid_t child;
		if((child=fork()) == -1){
			printf("Cannot fork process %d!\n", i);
			exit(EXIT_FAILURE);
		}	
		
		if(child==0){
		
			if(i<commandNo-1){
                close(pipes[i%2][0]);
                if(dup2(pipes[i%2][1], STDOUT_FILENO)<0){
                	printf("Cannot set writing on command %d!\n",i);
                    exit(EXIT_FAILURE);
                }
            }
            
            if(i!=0){
                close(pipes[(i+1)%2][1]);
                if(dup2(pipes[(i+1)%2][0], STDIN_FILENO)<0){
                	printf("Cannot set reading on command %d!\n",i);
                    close(EXIT_FAILURE);
                }
			}
			
			execvp(arguments[0], arguments);
			exit(EXIT_FAILURE);
		}
	}
	close(pipes[i % 2][0]);
    close(pipes[i % 2][1]);
    wait(NULL);
	exit(EXIT_SUCCESS);
	

}

int main(int argc, char **argv){

	if(argc<2){
		printf("Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
	}
	
	FILE* file = fopen(argv[1],"r");
	if(file==NULL){
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	
	char* line = NULL;
	size_t length = 0;
	int status;
	int lineCount=0;
	while(getline(&line,&length,file)!=-1){
		lineCount++;
		

		pid_t child_pid = fork();
		if(child_pid==0){
			execLine(line,lineCount);
			exit(EXIT_FAILURE);
		}
		
		wait(&status);
		if(status==0){
			continue;
		}else{
			printf("Cannot run command in line %d\n",lineCount);
			exit(EXIT_FAILURE);
		}
	
	}

	fclose(file);
	return 0;
}
