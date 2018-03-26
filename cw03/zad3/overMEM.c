#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **args){
	char** array = malloc(sizeof(char*) * 999999999);
    for (int i = 0; i< 999999999; i++) {
        array[i] = "allocated";
}
	
	return 0;
}
