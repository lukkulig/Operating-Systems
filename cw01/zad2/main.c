#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <unistd.h>

#ifndef DLL
#include "./../zad1/blocksarray.h"
#endif

#ifdef DLL
void *lib;
#endif

char* generateString(int size);

void executeOperationDynamic(char* operation,int parameter, char** arr, int numOfBlocks, int blockSize); //dynamic
void executeOperationStatic(char* operation,int parameter, int blockSize); //static
double timeDifference(clock_t begin, clock_t end);
void printTimes(clock_t begin, clock_t end, struct tms tms_begin, struct tms tms_end, char* operation);


int main(int argc, char **args){
    /*
    argv[0] - number of blocks
    argv[1] - block size
    argv[2] - allocation mode (static / dynamic)
    argv[3] - operations (max 3) {find, del_then_add, del_and_add}
    */
    srand(time(NULL));
    
    #ifdef DLL
    lib=dlopen("./../zad1/libblocksarrayshared.so", RTLD_LAZY);
    if(!lib){
		    printf("%s\n", "Cannot open library!");
		    return 1;
		}
    #endif
    
    
    if(argc<3 || argc>10){
        printf("Wrong number of arguments!\n");
        return 1;
    }

    int numberOfBlocks = atoi(args[1]);
    int blockSize = atoi(args[2]);

    bool isStatic;
    if(strcmp(args[3],"static")==0)
        isStatic = true;
    else if(strcmp(args[3],"dynamic")==0)
        isStatic = false;
    else{
        printf("Wrong allocation mode!\n");
        return 1;
    }

    clock_t clocks[8] = {0,0,0,0,0,0,0,0};
    struct tms *time[8];
    for(int i=0;i<8;i++){
        time[i] = (struct tms *)calloc(1,sizeof(struct tms *));
    }

    printf("\nNo of blocks: %d  Block size: %d  Allocation: %s\n",
           numberOfBlocks,blockSize,args[3]);
     
    clocks[0] = times(time[0]);


    if(isStatic){
    	#ifdef DLL
    	bool (*createStaticArray)(int numOfBlocks, int blockSize) = dlsym(lib,"createStaticArray");
    	bool (*addBlockStatic)(int blockIndex, char* blockContent) = dlsym(lib,"addBlockStatic");
    	#endif
        createStaticArray(numberOfBlocks,blockSize);
        for(int i=0; i<numberOfBlocks; i++){
            char* tmp = generateString(blockSize);
            addBlockStatic(i,tmp);
        }
       	
        clocks[1] = times(time[1]);
		printTimes(clocks[0],clocks[1],*time[0],*time[1],"create table");

        if(argc==6 || argc==8 || argc==10){
        	clocks[2] = times(time[2]);
            executeOperationStatic(args[4],atoi(args[5]),blockSize);
            clocks[3] = times(time[3]);
            printTimes(clocks[2],clocks[3],*time[2],*time[3],args[4]);
        }
        if(argc==8 || argc==10){
        	clocks[4] = times(time[4]);
            executeOperationStatic(args[6],atoi(args[7]),blockSize);
            clocks[5] = times(time[5]);
            printTimes(clocks[4],clocks[5],*time[4],*time[5],args[6]);
        }
        if(argc==10){
        	clocks[6] = times(time[6]);
            executeOperationStatic(args[8],atoi(args[9]),blockSize);
            clocks[7] = times(time[7]);
            printTimes(clocks[6],clocks[7],*time[6],*time[7],args[8]);
        }

		#ifdef DLL
    	void (*removeStaticArray)() = dlsym(lib,"removeStaticArray");
    	#endif
    	
        removeStaticArray();

    }else{
    	#ifdef DLL
		char** (*createDynamicArray)(int numOfBlocks) = dlsym(lib,"createDynamicArray");		
		bool (*addBlockDynamic)(char** charArray,int numOfBlocks,int blockIndex, char* blockContent) = dlsym(lib,"addBlockDynamic");
		#endif
		
        char** arr = createDynamicArray(numberOfBlocks);
        for(int i=0; i<numberOfBlocks; i++){
            char* tmp = generateString(blockSize);
            addBlockDynamic(arr,numberOfBlocks,i,tmp);
        }

        clocks[1] = times(time[1]);
		printTimes(clocks[0],clocks[1],*time[0],*time[1],"create table");
		
        if(argc==6 || argc==8 || argc==10){
        	clocks[2] = times(time[2]);
            executeOperationDynamic(args[4],atoi(args[5]),arr,numberOfBlocks,blockSize);
            clocks[3] = times(time[3]);
            printTimes(clocks[2],clocks[3],*time[2],*time[3],args[4]);
        }
        if(argc==8 || argc==10){
        	clocks[4] = times(time[4]);
            executeOperationDynamic(args[6],atoi(args[7]),arr,numberOfBlocks,blockSize);
            clocks[5] = times(time[5]);
            printTimes(clocks[4],clocks[5],*time[4],*time[5],args[6]);
        }
        if(argc==10){
        	clocks[6] = times(time[6]);
            executeOperationDynamic(args[8],atoi(args[9]),arr,numberOfBlocks,blockSize);
            clocks[7] = times(time[7]);
            printTimes(clocks[6],clocks[7],*time[6],*time[7],args[8]);
        }
		#ifdef DLL
		void (*removeDynamicArray)(char** charArray, int numOfBlocks) = dlsym(lib,"removeDynamicArray");
		#endif
		
        removeDynamicArray(arr,numberOfBlocks);
    }
	#ifdef DLL
  	dlclose(lib);
	#endif
    return 0;
}

void executeOperationDynamic(char* operation,int parameter, char** arr, int numOfBlocks, int blockSize){
    if(strcmp(operation,"del_then_add")==0){
    	#ifdef DLL
  		bool (*removeBlockDynamic)(char** charArray,int numOfBlocks,int blockIndex) = dlsym(lib,"removeBlockDynamic");
  		bool (*addBlockDynamic)(char** charArray,int numOfBlocks,int blockIndex, char* blockContent) = dlsym(lib,"addBlockDynamic");
		#endif
        for(int i=0;i<parameter;i++){
            removeBlockDynamic(arr,numOfBlocks,i);
        }
        for(int i=0;i<parameter;i++){
            char* tmp = generateString(blockSize);
            addBlockDynamic(arr,numOfBlocks,i,tmp);
        }

    } else if(strcmp(operation,"del_and_add")==0){
    	#ifdef DLL
  		bool (*removeBlockDynamic)(char** charArray,int numOfBlocks,int blockIndex) = dlsym(lib,"removeBlockDynamic");
  		bool (*addBlockDynamic)(char** charArray,int numOfBlocks,int blockIndex, char* blockContent) = dlsym(lib,"addBlockDynamic");
		#endif
        for(int i=0;i<parameter;i++){
                char* tmp = generateString(blockSize);
                removeBlockDynamic(arr,numOfBlocks,i);
                addBlockDynamic(arr,numOfBlocks,i,tmp);
        }
    } else if(strcmp(operation,"find")==0){
    	#ifdef DLL
  		int (*findBlockDynamic)(char** charArray,int numOfBlocks,int findSum) = dlsym(lib,"findBlockDynamic");
		#endif
        findBlockDynamic(arr,numOfBlocks, parameter);
    }
}

void executeOperationStatic(char* operation,int parameter, int blockSize){
    if(strcmp(operation,"del_then_add")==0){
    	#ifdef DLL
  		bool (*removeBlockStatic)(int blockIndex) = dlsym(lib,"removeBlockStatic");
  		bool (*addBlockStatic)(int blockIndex, char* blockContent) = dlsym(lib,"addBlockStatic");
		#endif
        for(int i=0;i<parameter;i++){
            removeBlockStatic(i);
        }
        for(int i=0;i<parameter;i++){
            char* tmp = generateString(blockSize);
            addBlockStatic(i,tmp);
        }

    } else if(strcmp(operation,"del_and_add")==0){
    	#ifdef DLL
  		bool (*removeBlockStatic)(int blockIndex) = dlsym(lib,"removeBlockStatic");
  		bool (*addBlockStatic)(int blockIndex, char* blockContent) = dlsym(lib,"addBlockStatic");
		#endif
        for(int i=0;i<parameter;i++){
                char* tmp = generateString(blockSize);
                removeBlockStatic(i);
                addBlockStatic(i,tmp);
        }
    } else if(strcmp(operation,"find")==0){
    	#ifdef DLL
  		int (*findBlockStatic)(int findSum) = dlsym(lib,"findBlockStatic");
		#endif
        findBlockStatic(parameter);
    }
}

void printTimes(clock_t begin, clock_t end, struct tms tms_begin, struct tms tms_end, char* operation){
	printf("%s\n", operation);
	printf("   Real      User      System\n");
	printf("%lf   ", timeDifference(end, begin));
    printf("%lf   ", timeDifference(tms_end.tms_utime, tms_begin.tms_utime));
	printf("%lf   ", timeDifference(tms_end.tms_stime, tms_begin.tms_stime));
	printf("\n");
}

double timeDifference(clock_t end, clock_t begin){
	return (double)(end-begin)/sysconf(_SC_CLK_TCK);
}

char* generateString(int size) {
    if (size < 1) return NULL;
    char* chars = "abcdefghijklmnopqrstuvwxyz";
    int len = strlen(chars);
    char *result = (char*)calloc(size,sizeof(char));

    for (int i=0; i<size-1; i++) {
        result[i] = chars[rand()%len];
    }
    result[size-1]='\0';
    return result;
}
