#ifndef BLOCKSARRAY_H
#define BLOCKSARRAY_H

#include <stdbool.h>

//tablica statyczna

bool createStaticArray(int numOfBlocks, int blockSize);

void removeStaticArray();

bool addBlockStatic(int blockIndex, char* blockContent);
bool removeBlockStatic(int blockIndex);
int findBlockStatic(int findSum);

void printStaticArray();

//tablica dynamiczna

char** createDynamicArray(int numOfBlocks);
void removeDynamicArray(char** charArray, int numOfBlocks);

bool addBlockDynamic(char** charArray, int numOfBlocks,int blockIndex, char* blockContent);
bool removeBlockDynamic(char** charArray,int numOfBlocks,int blockIndex);
int findBlockDynamic(char** charArray,int numOfBlocks,int findSum);

void printDynamicArray(char** charArray,int numOfBlocks);

#endif // BLOCKSARRAY_H
