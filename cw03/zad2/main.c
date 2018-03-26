#include <stdio.h>
#include <stdlib.h>


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
	
	
	
	fclose(file);
	exit(EXIT_SUCCESS);
}
