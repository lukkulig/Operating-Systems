#include "blocksarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//tablica statyczna

#define MAX_NUM_OF_BLOCKS 100000
#define MAX_BLOCK_SIZE 20000

char CHAR_ARRAY[MAX_NUM_OF_BLOCKS][MAX_BLOCK_SIZE];
bool TAKEN[MAX_NUM_OF_BLOCKS];

int NUM_OF_BLOCKS = 0;
int BLOCK_SIZE = 0;

bool createStaticArray(int numOfBlocks, int blockSize){
    if(numOfBlocks<=MAX_NUM_OF_BLOCKS && blockSize<=MAX_BLOCK_SIZE){

        NUM_OF_BLOCKS = numOfBlocks;
        BLOCK_SIZE = blockSize;
        

        for(int i=0; i<numOfBlocks;i++)
            TAKEN[i] = false;
        return true;
    }
    return false;
}


void removeStaticArray(){
    NUM_OF_BLOCKS = 0;
    BLOCK_SIZE = 0;
}

bool addBlockStatic(int blockIndex, char* blockContent){
    if(blockIndex<NUM_OF_BLOCKS && !TAKEN[blockIndex] && strlen(blockContent)<=BLOCK_SIZE){

        TAKEN[blockIndex] = true;
        strcpy(CHAR_ARRAY[blockIndex], blockContent);
        return true;
    }
    return false;
}

bool removeBlockStatic(int blockIndex){
    if(blockIndex<NUM_OF_BLOCKS){

        TAKEN[blockIndex]=false;
        return true;
    }
    return false;
}

int findBlockStatic(int findSum){
    int result=-1;
    int eps = INT_MAX;
    
    for(int i=0;i<NUM_OF_BLOCKS;i++){
         if(TAKEN[i]){
            int sum=0;
            for(int idx=0;idx<BLOCK_SIZE;idx++)
                sum+=CHAR_ARRAY[i][idx];

            int tmpEps = abs(findSum - sum);
            if(tmpEps<eps){
                eps = tmpEps;
                result=i;
            }
        }
    }
    return result;


}

void printStaticArray(){
    if(NUM_OF_BLOCKS>0){
        printf("Tablica statyczna:\n");
        for(int i=0; i<NUM_OF_BLOCKS;i++){
            printf("\t%d : ",i);
            if(TAKEN[i])
                printf("%s \n",CHAR_ARRAY[i]);
            else
                printf("NULL \n");
        }
        printf("\n");
    }
}

//tablica dynamiczna

char** createDynamicArray(int numOfBlocks){
    return (char**)calloc(numOfBlocks,sizeof(char*));
}

void removeDynamicArray(char** charArray, int numOfBlocks){
    for(int i=0;i<numOfBlocks;i++){
        free(charArray[i]);
    }
    free(charArray);
}

bool addBlockDynamic(char** charArray,int numOfBlocks,int blockIndex, char* blockContent){
    if(blockIndex<numOfBlocks && charArray[blockIndex]==NULL){

        charArray[blockIndex] = (char*)calloc(strlen(blockContent),sizeof(char));
        strcpy(charArray[blockIndex], blockContent);
        return true;
    }

    return false;
}

bool removeBlockDynamic(char** charArray,int numOfBlocks,int blockIndex){
    if(blockIndex<numOfBlocks){

        free(charArray[blockIndex]);
        charArray[blockIndex]=NULL;
        return true;
    }

    return false;
}

int findBlockDynamic(char** charArray,int numOfBlocks,int findSum){
    int result=-1;
    int eps = INT_MAX;

    for(int i=0;i<numOfBlocks;i++){
         if(charArray[i]!=NULL){
            int sum=0;
            int strLength= strlen(charArray[i]);
            for(int idx=0;idx<strLength;idx++)
                sum+=charArray[i][idx];

            int tmpEps = abs(findSum - sum);
            if(tmpEps<eps){
                eps = tmpEps;
                result=i;
            }
        }
    }
    return result;
}

void printDynamicArray(char** charArray,int numOfBlocks){
    printf("Tablica dynamiczna:\n");
    for(int i=0; i<numOfBlocks;i++){
        printf("\t%d : %s \n",i,charArray[i]);
    }
    printf("\n");
}

