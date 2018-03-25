#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>

bool generate(char* fileName,int numOfRecords,int recordLength);
bool sort_lib(char* fileName,int numOfRecords,int recordLength);
bool sort_sys(char* fileName,int numOfRecords,int recordLength);
bool copy_lib(char* fileFrom,char* fileTo,int numOfRecords,int recordLength);
bool copy_sys(char* fileFrom,char* fileTo,int numOfRecords,int recordLength);

double timeDifference(clock_t end, clock_t begin){
	return (double)(end-begin)/sysconf(_SC_CLK_TCK);
}
void printTimes(clock_t begin, clock_t end, struct tms tms_begin, struct tms tms_end, char* operation);


int main(int argc, char **args){

	clock_t clocks[2] = {0,0};
	struct tms *time[2];
	for(int i=0;i<2;i++){
	    time[i] = (struct tms *)calloc(1,sizeof(struct tms *));
	}

	switch(argc){
		case 5: //generate
			if(strcmp(args[1],"generate")== 0){
				char* fileName=args[2];
				int numOfRecords = atoi(args[3]);
				int recordLength = atoi(args[4]);
				if(!generate(fileName,numOfRecords,recordLength)){
					printf("Cannot generate!\n");
				}
			}			
			break;
		case 6: //sort
			if(strcmp(args[1],"sort")== 0){
			
			
				clocks[0]=times(time[0]);
			
				char* fileName=args[2];
				int numOfRecords = atoi(args[3]);
				int recordLength = atoi(args[4]);
				if(strcmp(args[5],"sys")== 0){
            		if(!sort_sys(fileName,numOfRecords,recordLength)){
            			printf("Sorting went wrong!\n");
            		}else{
            			clocks[1]=times(time[1]);
						printTimes(clocks[0],clocks[1],*time[0],*time[1],"sorting sys");
            		}
        		}else if(strcmp(args[5],"lib")== 0){
            		if(!sort_lib(fileName,numOfRecords,recordLength)){
            			printf("Sorting went wrong!\n");
            		}else{
            			clocks[1]=times(time[1]);
						printTimes(clocks[0],clocks[1],*time[0],*time[1],"sorting lib");
            		}
        		}else{
            		printf("Wrong sorting mode!\n");
				}
				
			}	
			break;
		case 7: //copy
			if(strcmp(args[1],"copy")== 0){
				clocks[0]=times(time[0]);
				char* fileFrom=args[2];
				char* fileTo=args[3];
				int numOfRecords = atoi(args[4]);
				int recordLength = atoi(args[5]);
				if(strcmp(args[6],"sys")== 0){
            		if(!copy_sys(fileFrom,fileTo,numOfRecords,recordLength)){
            			printf("Copying went wrong!\n");
            		}else{
            			clocks[1]=times(time[1]);
						printTimes(clocks[0],clocks[1],*time[0],*time[1],"copying sys");
            		}
        		}else if(strcmp(args[6],"lib")== 0){
            		if(!copy_lib(fileFrom,fileTo,numOfRecords,recordLength)){
            			printf("Copying went wrong!\n");
            		}else{
            			clocks[1]=times(time[1]);
						printTimes(clocks[0],clocks[1],*time[0],*time[1],"copying lib");
            		}
        		}else{
            		printf("Wrong copying mode!\n");
				}
			}	
			break;
		default:
			printf("Wrong number of arguments!\n");
        	return 1;	
	}	
	return 0;
}

bool generate(char* fileName,int numOfRecords,int recordLength){
	FILE *file = fopen(fileName,"w");
	if(file==NULL)
		return false;
//	FILE *rand = fopen("/dev/random","r");
//	if(rand==NULL)
//		return false; 
	char *record = (char*)malloc(recordLength*sizeof(char));
	for(int i=0;i<numOfRecords;i++){
//		if(fread(record,sizeof(char),(size_t)recordLength,rand)!= recordLength){
//			return false;
//		}
		for(int j=0;j<recordLength-1;j++){
			//record[j]=(char)(abs(record[j])%25+65);
			record[j]=rand()%25+65;
		}
		record[recordLength-1]='\n';
		if(fwrite(record,sizeof(char),(size_t)recordLength,file)!=recordLength){
			return false;
		}
		
	}
	fclose(file);
	//fclose(rand);
	free(record);
	return true;
	
}

bool copy_lib(char* fileFrom,char* fileTo,int numOfRecords,int recordLength){
	FILE *source=fopen(fileFrom,"rw");
	if(source==NULL)
		return false;
	FILE *copy=fopen(fileTo,"w+");
	if(copy==NULL)
		return false;
	
	char* buffor=(char*)malloc(recordLength*sizeof(char));
	
	for(int i=0;i<numOfRecords;i++){
		if(fread(buffor,sizeof(char),(size_t)recordLength,source)!=recordLength){
			return false;
		}
		if(fwrite(buffor,sizeof(char),(size_t)recordLength,copy)!=recordLength){
			return false;
		}
	}
	
	fclose(source);
	fclose(copy);	
	return true;
}
bool copy_sys(char* fileFrom,char* fileTo,int numOfRecords,int recordLength){
	int source=open(fileFrom,O_RDONLY);
	if(source<0)
		return false;
	int copy=open(fileTo,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
	if(copy<0)
		return false;
	
	char* buffor=(char*)malloc(recordLength*sizeof(char));
	
	for(int i=0;i<numOfRecords;i++){
		if(read(source,buffor,sizeof(char)*(size_t)recordLength)!=recordLength){
			return false;
		}
		if(write(copy,buffor,sizeof(char)*(size_t)recordLength)!=recordLength){
			return false;
		}
	}
	free(buffor);
	close(source);
	close(copy);	
	return true;
}

bool sort_lib(char* fileName,int numOfRecords,int recordLength){
	FILE *file=fopen(fileName,"rw+");
	if(file==NULL)
		return false;
	
	int offset=sizeof(char)*recordLength;
	char* insert=(char*)malloc(recordLength*sizeof(char));
	char* swap=(char*)malloc(recordLength*sizeof(char));
	
	for(int i=0;i<numOfRecords;i++){
		if(fseek(file,i*offset,0)!=0){
			return false;
		}
		if(fread(insert,sizeof(char),(size_t)recordLength,file)!=recordLength){
			return false;
		}
		for(int j=0;j<i;j++){
			if(fseek(file,j*offset,0)!=0){
				return false;
			}
			if(fread(swap,sizeof(char),(size_t)recordLength,file)!=recordLength){
				return false;
			}
			 if(insert[0]<swap[0]){
			 	if(fseek(file,j*offset,0)!=0){
				return false;
			}
			 	if(fwrite(insert,sizeof(char),(size_t)recordLength,file)!=recordLength){
					return false;
				}
				if(fseek(file,i*offset,0)!=0){
				return false;
			}
			 	if(fwrite(swap,sizeof(char),(size_t)recordLength,file)!=recordLength){
					return false;
				}
				char* tmp=insert;
				insert=swap;
				swap=tmp;
			 }			
		}
		
	}
	free(insert);
	free(swap);
	fclose(file);
	return true;
}
bool sort_sys(char* fileName,int numOfRecords,int recordLength){
	int file=open(fileName,O_RDWR);
	if(file<0)
		return false;
	
	int offset=sizeof(char)*recordLength;
	char* insert=(char*)malloc(recordLength*sizeof(char));
	char* swap=(char*)malloc(recordLength*sizeof(char));
	for(int i=0;i<numOfRecords;i++){
		if(lseek(file,i*offset,SEEK_SET)<0){
			return false;
		}
		if(read(file,insert,(size_t)recordLength)!=recordLength){
			return false;
		}
		for(int j=0;j<i;j++){
			if(lseek(file,j*offset,SEEK_SET)<0){
				return false;
			}
			if(read(file,swap,(size_t)recordLength)!=recordLength){
			return false;
		}
			 if(insert[0]<swap[0]){
			 	if(lseek(file,j*offset,SEEK_SET)<0){
					return false;
				}
			 	if(write(file,insert,sizeof(char)*recordLength)!=recordLength){
					return false;
				}
				if(lseek(file,i*offset,SEEK_SET)<0){
					return false;
				}
			 	if(write(file,swap,sizeof(char)*recordLength)!=recordLength){
					return false;
				}
				char* tmp=insert;
				insert=swap;
				swap=tmp;
			 }			
		}
		
	}
	free(insert);
	free(swap);
	close(file);
	return true;
}

void printTimes(clock_t begin, clock_t end, struct tms tms_begin, struct tms tms_end, char* operation){
	printf("%s\n", operation);
	printf("   Real      User      System\n");
	printf("%lf   ", timeDifference(end, begin));
    printf("%lf   ", timeDifference(tms_end.tms_utime, tms_begin.tms_utime));
	printf("%lf   ", timeDifference(tms_end.tms_stime, tms_begin.tms_stime));
	printf("\n\n");
}

