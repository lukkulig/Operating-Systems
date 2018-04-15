#include <stdio.h>

#define MAX_LINE_LENGTH 256

int main(int argc, char **argv) {

	if(argc!=3){
		printf("Wrong number of arguments! Using: ./test \"command 1 [-args]\" \"command 2 [-args]\"\n");
	}
    
    FILE* file1 = popen(argv[1], "r");
    FILE* file2 = popen(argv[2], "w");
    
    
    char buffer[MAX_LINE_LENGTH];    
    while(fgets(buffer, MAX_LINE_LENGTH, file1)){
        fputs(buffer,file2);        
    }
    
    fclose(file1);
    fclose(file2);
    return 0;
    
}
