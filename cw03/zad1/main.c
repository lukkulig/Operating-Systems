#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>

time_t date;
char* operator;

const char format[] = "%Y-%m-%d %H:%M:%S";

int compareDates(time_t date1, time_t date2) {
	int y1,y2,m1,m2,d1,d2;
	
	struct tm* date = localtime(&date1);
	y1 = date -> tm_year;
	m1 = date -> tm_mon;
	d1 = date -> tm_mday;
	date = localtime(&date2);
	y2 = date -> tm_year;
	m2 = date -> tm_mon;
	d2 = date -> tm_mday;
	if(y1>y2) return 1;
	if(y1<y2) return -1;
	if(m1>m2) return 1;
	if(m1<m1) return -1;
	if(d1>d2) return 1;
	if(d1<d2) return -1;
	return 0;
}

void printInfo(const char* fpath, const struct stat *sb){
	printf("%lld\t",(long long int)sb->st_size);
    printf((sb->st_mode & S_IRUSR) ? "r" : "-");
    printf((sb->st_mode & S_IWUSR) ? "w" : "-");
    printf((sb->st_mode & S_IXUSR) ? "x" : "-");
    printf((sb->st_mode & S_IRGRP) ? "r" : "-");
    printf((sb->st_mode & S_IWGRP) ? "w" : "-");
    printf((sb->st_mode & S_IXGRP) ? "x" : "-");
    printf((sb->st_mode & S_IROTH) ? "r" : "-");
    printf((sb->st_mode & S_IWOTH) ? "w" : "-");
	printf((sb->st_mode & S_IXOTH) ? "x" : "-");
	printf("\t%s\t", ctime(&(sb->st_mtime)));
	printf("%s\t", realpath(fpath, NULL));
	printf("\n\n");
}

static int display(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	
	int compRes=compareDates(date,sb->st_mtime);
	if(!(compRes==0 && strcmp(operator,"=")==0) &&
		!(compRes>0 && strcmp(operator,"<")==0) &&
		!(compRes<0 && strcmp(operator,">")==0)){
		return 0;
	}
	if(typeflag==FTW_F)
    	printInfo(fpath,sb);
    return 0;
}
//Zmodyfikuj zadanie 2 z poprzedniego zestawu w taki sposób, iż przeszukiwanie w każdym z odnalezionych (pod)katalogow powinno odbywać sie w osobnym procesie
void myNftw(const char *dirpath){
        if(dirpath==NULL) return;
        DIR *handle = opendir(dirpath);     
        if(handle==NULL) return;
        
        struct dirent *currentDir = readdir(handle);
        struct stat sb;
        
        char newdir[PATH_MAX];
        
        while(currentDir!=NULL){
        	strcpy(newdir,dirpath);
        	strcat(newdir,"/");
        	strcat(newdir, currentDir ->d_name);
        	lstat(newdir,&sb);
        	if(strcmp(currentDir->d_name,".")==0 || strcmp(currentDir->d_name,"..")==0){
			    currentDir = readdir(handle);
			    continue;
			}else{
	    		if(S_ISDIR(sb.st_mode)){
					pid_t forkRes = fork();
					if(forkRes==0){
						myNftw(newdir);	
						exit(0);	
					}else if(forkRes<0){
						exit(EXIT_FAILURE);
					}
				}
				if(S_ISREG(sb.st_mode)){
					display(newdir,&sb,FTW_F,NULL);
				}
			}
			currentDir = readdir(handle);
        
        }
        
        closedir(handle);
}

int main(int argc, char **args){

	if(argc!=4){
		printf("Wrong number of arguments!\n");
		return 1;
	}
	
	char* path=args[1];
	operator=args[2];
	char* sdate=args[3];
	
	struct tm* time = malloc(sizeof(struct tm));
	strptime(sdate,format,time);
	date = mktime(time);
	
	DIR *dir = opendir(path);
	if (dir == NULL) {
        printf("Cannot open dir!\n");
        return 1;
	}	

	//nftw(path, display, 10, FTW_F);
	//printf("\n\n\n");
	myNftw(path);

	closedir(dir);
	return 0;
}
