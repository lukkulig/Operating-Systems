#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

void parseInput(const char*);
void parseFilter(const char*);
void saveOutputImage(const char*);
void freeImageArrs();
void freeFilterArr();
void* filterImage(void* arg);
int filterPixel(int x, int y);

int imageWidth = -1,imageHeight = -1,filterDim = -1;
int threadsAmount;
int** I,**J;
double** K;
int **arg;

int main(int argc, char** argv) {

	if (argc != 5) {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
	}
	threadsAmount = (int) strtol(argv[1], 0, 10);

	parseInput(argv[2]);
	parseFilter(argv[3]);

	J = calloc(imageHeight, sizeof(int*));
    for(int i = 0; i < imageHeight; i++)
    {
        J[i] = calloc(imageWidth, sizeof(int));
    }

    int res;
    arg = malloc(threadsAmount * sizeof(int*));

    struct timeval startTime;
    gettimeofday(&startTime, NULL);

    pthread_t *threads = calloc(threadsAmount,sizeof(pthread_t));
    for(int i = 0; i < threadsAmount; i++)
    {
        arg[i] = malloc(sizeof(int));
        *arg[i] = i;

        res = pthread_create(&threads[i], NULL, &filterImage, (void *)arg[i]);
        if(res != 0){
        	errno = res;
         	perror("pthreat_create");
         	exit(EXIT_FAILURE);
        }
	}


	for(int i = 0; i < threadsAmount; i++)
    {
        res = pthread_join(threads[i], NULL);
        if(res != 0){
        	errno = res;
         	perror("pthread_join");
         	exit(EXIT_FAILURE);
        }
	}

	struct timeval endTime;
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &endTime);
    printf("Threads amount:\t\t%d\nFilter dimension:\t%d\nReal time:\t\t%ld.%06ld sec\n\n",threadsAmount, filterDim, endTime.tv_sec, endTime.tv_usec);

	saveOutputImage(argv[4]);

	freeImageArrs();
	freeFilterArr();
	free(threads);
	for(int i = 0; i < threadsAmount; i++)
        free(arg[i]);
    free(arg);
	return 0;

}

void* filterImage(void* threadNo){
	int currRow = *(int *)threadNo;
	while(currRow < imageHeight){
		for(int i = 0; i < imageWidth; i++){
			J[currRow][i] = filterPixel(currRow,i);
		}
		currRow += threadsAmount;
	}
	return 0;
}

int filterPixel(int x, int y){
	double s = 0.0;
	for(int i = 0; i < filterDim; i++)
		for(int j = 0; j < filterDim; j++){
			int a = max(0,min(imageHeight-1,(x-(int)ceil(filterDim/2.0)+i)));
			int b = max(0,min(imageWidth-1,(y-(int)ceil(filterDim/2.0)+j)));

			s+=I[a][b]*K[i][j];
		}
	return round(s);
}

void parseInput(const char* filename){
	FILE *inFile = fopen(filename, "r");
    if (inFile == NULL) {
        perror("Input file\n");
        exit(EXIT_FAILURE);
    }
    int maxVal;
    if((fscanf(inFile, "P2 %d %d %d", &imageWidth,&imageHeight,&maxVal)!= 3) || imageWidth < 1 || imageHeight < 1 || maxVal != 255){
    	printf("Wrong format of input file header!\n");
    	exit(EXIT_FAILURE);
    }

    int tmp;
    I = calloc(imageHeight, sizeof(int*));
    for(int i = 0; i < imageHeight; i++)
    {
        I[i] = calloc(imageWidth, sizeof(int));
        for(int j = 0; j < imageWidth; j++){
        	if((fscanf(inFile, "%d", &tmp) == 1) && tmp >= 0 && tmp <= maxVal)
        	{
    			I[i][j] = tmp;
        	}else{
        		printf("Wrong format of input file values!\n");
        		exit(EXIT_FAILURE);
        	}

        }
	}


	fclose(inFile);
}

void parseFilter(const char* filename){
	FILE *filterFile = fopen(filename, "r");
    if (filterFile == NULL) {
        perror("Filter file\n");
        exit(EXIT_FAILURE);
    }
    if((fscanf(filterFile, "%d", &filterDim) != 1) || filterDim < 1){
    	printf("Wrong format of filter file header!\n");
		exit(EXIT_FAILURE);
    }

    double tmp;
    K = calloc(filterDim, sizeof(double*));
    for(int i = 0; i < filterDim; i++)
    {
        K[i] = calloc(filterDim, sizeof(double));
        for(int j = 0; j < filterDim; j++)
        {
            if(fscanf(filterFile, "%lf", &tmp) == 1){
                K[i][j] = tmp;
           	}else{
           		printf("Wrong format of filter file values!\n");
				exit(EXIT_FAILURE);
           	}
		}
	}

	fclose(filterFile);

}

void saveOutputImage(const char* filename){
	FILE *outFile = fopen(filename, "w");
    if (outFile == NULL) {
        perror("Output file\n");
        exit(EXIT_FAILURE);
	}
	fprintf(outFile, "P2\n%d %d\n255\n", imageWidth, imageHeight);

	for(int i = 0; i < imageHeight; i++){
        for(int j = 0; j < imageWidth; j++){
            fprintf(outFile, "%d ", J[i][j]);
        }
        fprintf(outFile, "\n");
	}

	fclose(outFile);

}

void freeImageArrs(){
	for(int i = 0; i < imageHeight; i++){
        free(I[i]);
        free(J[i]);
    }
    free(I);
	free(J);
}
void freeFilterArr(){
	for(int i = 0; i < filterDim; i++)
        free(K[i]);
	free(K);
}
