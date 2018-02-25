#define FUSE_USE_VERSION 26

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/xattr.h>
#include <dirent.h>
#include <unistd.h>
#include <fuse.h>
#include "libcsv-3.0.3/libcsv.c"



static const char *fs_path = "postal-codes.csv";

struct Information {
	char code[30];
	char neighborhood[30];
	char plaka[3];
	char city[30];
	char district[30];
	char latitude[30];
	char longitude[30];
};

struct Information infos[40000];
char cities[82][30];
int counterCity=1;
int counterInfos=0;

char search[30];//örneğin şehir içerisindeki ilçeleri aramak için kullanılacak. Şehir ismi bu değişkene atanacak. İlgili İlçeler temp dizisine eklenecek, sayaç artırılacak.
char searchDistrict[30];
char searchNeighborhood[30];
char code[30];
int globalCounter;//bir klasör için içindeki elemanları listeneceğinde, kaç tane eleman olduğunu tutacak.

void cb1 (void *s, size_t i, void *outfile) {
	char *token;
	char *search = "\t";
	char *temp = strdup(s);
	char *result;
    token=strsep(&temp, "\t");	
    token=strsep(&temp, "\t");	
    token=strsep(&temp, "\t");
    if(strcmp(cities[counterCity-1],token)!=0){
		strcpy(cities[counterCity],token);
		counterCity++;
	}
	
	temp = strdup(s);
	char* code=strsep(&temp, search);
	char plaka[3];
	plaka[0] = code[0];
	plaka[1] = code[1];
	plaka[2] = '\0';
    char* neighborhood=strsep(&temp, search);	
    char* city=strsep(&temp, search);	
    char* district=strsep(&temp, search);	
    char* latitude=strsep(&temp, search);	
    char* longitude=temp;
    strcpy(infos[counterInfos].code,code);
	strcpy(infos[counterInfos].neighborhood,neighborhood);
	strcpy(infos[counterInfos].city,city);
	strcpy(infos[counterInfos].district,district);
	strcpy(infos[counterInfos].latitude,latitude);
	strcpy(infos[counterInfos].longitude,longitude);
	strcpy(infos[counterInfos].plaka,plaka);
	counterInfos++;
}

void cb2 (int c, void *outfile) {
  fseek((FILE *)outfile, -1, SEEK_CUR);
  fputc('\n', (FILE )outfile);
}

void readCSVFile(){
	char buf[1024];
	size_t i;
	struct csv_parser p;
	FILE *infile, *outfile;
	csv_init(&p, 0);

	infile = fopen("postal-codes.csv", "rb");
	if (infile == NULL) {
		fprintf(stderr, "Failed to open file postal-codes.csv: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	outfile = fopen("success.csv", "wb");
	if (outfile == NULL) {
		fprintf(stderr, "Failed to open file success.csv: %s\n", strerror(errno));
		fclose(infile);
		exit(EXIT_FAILURE);
	}

	while ((i=fread(buf, 1, 1024, infile)) > 0) {
		if (csv_parse(&p, buf, i, cb1, cb2, outfile) != i) {
		fprintf(stderr, "Error parsing file: %s\n", csv_strerror(csv_error(&p)));
		fclose(infile);
		fclose(outfile);
		remove("postal-codes.csv");
		exit(EXIT_FAILURE);
		}
	}

	csv_fini(&p, cb1, cb2, outfile);
	csv_free(&p);

	if (ferror(infile)) {
		fprintf(stderr, "Error reading from input file");
		fclose(infile);
		fclose(outfile);
		remove("success.csv");
		exit(EXIT_FAILURE);
	}

	fclose(infile);
	fclose(outfile);	
	
}

static int fs_getattr(const char *path, struct stat *stbuf)
{
	
	//printf("Counter basladi \n");
	
    int res = 0;
	int count = 0,i = 0;
	char *tempPath=strdup(path);
	//printf("path[0]: %c\n", tempPath[i]);
	int size = strlen(tempPath);
	//printf("size %d\n", size);
	while(i<size){
		//printf("while - path: %s \n", tempPath);
		if(tempPath[i] == '/'){
			count++ ;
			
		}
		i++;
	}
    //memset(stbuf, 0, sizeof(struct stat));
    stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
    
    
    if (strcmp(tempPath, "/") == 0) {
		
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 4;
        
    }else if(strncmp(tempPath, "/CODES",6) == 0){
		if(count == 1){
			stbuf->st_mode = S_IFDIR | 0777; 
			stbuf->st_nlink = 83;
		}else if(count == 2){
			stbuf->st_mode = S_IFDIR | 0777; 	//  /CODES/34
			stbuf->st_nlink = 2;
		}else if(count == 3){
			stbuf->st_mode = S_IFREG | 0777; 	//  /CODES/34/34567.txt
			stbuf->st_nlink = 1;
		}else{
			return -ENOENT;
		}
		
	}else if(strncmp(tempPath, "/NAMES",6) == 0){
		if(count == 1){
			stbuf->st_mode = S_IFDIR | 0777; 
			stbuf->st_nlink = 83;
		}else if(count == 2){
			stbuf->st_mode = S_IFDIR | 0777; 	//  /NAMES/ISTANBUL  # burası degisecek dinamik
			stbuf->st_nlink = 2;
		}else if(count == 3){
			stbuf->st_mode = S_IFDIR | 0777; 	//  /NAMES/ISTANBUL/SARIYER
			stbuf->st_nlink = 2;
		}else if(count == 4){
			stbuf->st_mode = S_IFREG | 0777; 	//  /NAMES/ISTANBUL/SARIYER/Maslak.txt
			stbuf->st_nlink = 1;
		}else{
			return -ENOENT;
		}
	}
    else
        res = -ENOENT;

    return res;
}

int ifContains(char arr[][30],char *text,int size){
	int flag = 0;
	int i = 0;
	for(i=0;i<size;i++){
		if(strcmp(text,arr[i]) == 0){
			flag = 1;
			return flag;
		}
	}	
	return flag;
	
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{

	char *tempPath = strdup(path);

	int count = 0,i = 0;
	
	int size = strlen(tempPath);
	while(i<size){
		if(tempPath[i] == '/'){
			count++ ;
			
		}
		i++;
	}
	
	
	printf("Counter hesaplandi %d \n",count);

	

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    
    if (strcmp(tempPath, "/") == 0){
	    filler(buf, "CODES", NULL, 0);
		filler(buf, "NAMES", NULL, 0);
	}else if(strncmp(tempPath, "/CODES",6) == 0){
		if(count == 1){		//	/CODES
			int i=0;
			char str[3];
			filler(buf, "01", NULL, 0);
			filler(buf, "02", NULL, 0);
			filler(buf, "03", NULL, 0);
			filler(buf, "04", NULL, 0);
			filler(buf, "05", NULL, 0);
			filler(buf, "06", NULL, 0);
			filler(buf, "07", NULL, 0);
			filler(buf, "08", NULL, 0);
			filler(buf, "09", NULL, 0);
			for(i=10;i<=81;i++){
				sprintf(str,"%d",i);
				filler(buf, str, NULL, 0);
			}	
		}
		else if(count == 2){	//	/CODES/34
			char tempArrayCode[10000][30];
			int tempArrayCount = 0 ;
			char *ptr = strdup(path);
			ptr++;
			strsep(&ptr,"/");  // ptr => 34 (code)
			int i;
			for(i=0;i<counterInfos;i++){
				if(strcmp(infos[i].plaka,ptr) == 0 && ifContains(tempArrayCode,infos[i].code,tempArrayCount) == 0){					
					strcpy( tempArrayCode[tempArrayCount] , infos[i].code );
					tempArrayCount++ ;
				}
			}			
			for(i=0;i<tempArrayCount;i++){
				filler( buf, tempArrayCode[i], NULL, 0 );
			}
			
		}
		
	}else if(strncmp(tempPath, "/NAMES",6) == 0){
		if(count == 1){				//   /NAMES
			int i;
			for(i=1;i<82;i++){
				filler( buf, cities[i], NULL, 0 );
			}
		}else if(count == 2){		//   /NAMES/Istanbul
			printf("count =2\n");
			char tempArrayDistrict[1000][30];
			int tempArrayCount = 0;
			
			char *cpPath = strdup(path);
			cpPath++ ;	
			char *names=strsep(&cpPath, "/");
			
			char *ptr = cpPath ; // ptr takes city (Istanbul))
			
			
			printf("names = %s , city = %s\n",names,ptr);
			int i;
			for(i=0;i<counterInfos;i++){
				if(strcmp(infos[i].city,ptr) == 0 ){
					if(ifContains(tempArrayDistrict,infos[i].district,tempArrayCount) == 0){					
						strcpy( tempArrayDistrict[tempArrayCount] , infos[i].district );
						tempArrayCount++ ;
						printf(" INSIDE-IF = city = %s , district = %s \n",infos[i].city,infos[i].district);
					}
				}
			}
			printf("iki forun arası dugun olmaz \n");
			for(i=0;i<tempArrayCount;i++){
				filler( buf, tempArrayDistrict[i], NULL, 0 );
				printf("%s eklendi\n",tempArrayDistrict[i]);
			}
			printf("mutlu son \n");
		}else if(count == 3){	//   /NAMES/Istanbul/Sarıyer
			
			char *cpPath = strdup(path);
			cpPath++ ;	
			char *names=strsep(&cpPath, "/");
			char *city = strsep(&cpPath, "/");
			
			
			char tempArrayNeighborhood[100][30];
			int tempArrayCount = 0;
			int i;
			for(i=0;i<counterInfos;i++){
				if(strcmp(infos[i].city,city) == 0 && strcmp(infos[i].district,cpPath) == 0 && ifContains(tempArrayNeighborhood,infos[i].neighborhood,tempArrayCount) == 0){
					strcpy( tempArrayNeighborhood[tempArrayCount] , infos[i].neighborhood );
					tempArrayCount++ ;
				}
			}
			for(i=0;i<tempArrayCount;i++){
				filler( buf, tempArrayNeighborhood[i], NULL, 0 );
			}
			
		}
	}



    return 0;
}

static int fs_open(const char *path, struct fuse_file_info *fi)
{



    return 0;
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	char *tempPath = strdup(path);
	int isNames=0;
	int returnSize=0;

	int count = 0,i = 0;
	
	int s = strlen(tempPath);
	while(i<s){
		if(tempPath[i] == '/'){
			count++ ;
			
		}
		i++;
	}
	
	char *emptyCode = "NULL";
	char *selectedText;
	if(count==4){
		//  /NAMES/Istanbul/Sariyer/Maslak.txt
		char *cpPath = strdup(path);		
		cpPath++ ;	
		char *names=strsep(&cpPath, "/");
		char *city = strsep(&cpPath, "/");
		char *district = strsep(&cpPath, "/"); // cpPath is neighborhood
		int i;
		for(i=0;i<counterInfos;i++){
			if(strcmp(infos[i].city,city) == 0 && strcmp(infos[i].district,district) == 0 && strcmp(infos[i].neighborhood,cpPath) == 0){
				printf("######### city: %s, district: %s, neighborhood: %s\n",city,district,cpPath);
				selectedText=infos[i].code;
				isNames=1;
				printf("######### selectedText: %s\n",selectedText);
				break;
			}
		}
		
	}
	
	if(isNames==1){
		int len = strlen(selectedText);
		if (offset < len) {
			if (offset + size > len)
				size = len - offset;
			memcpy(buf, selectedText + offset, size);
		} else
			size = 0;
	}else{
		int len = strlen(emptyCode);
		if (offset < len) {
			if (offset + size > len)
				size = len - offset;
			memcpy(buf, emptyCode + offset, size);
		} else
			size = 0;
	}
	return size;
}



struct fuse_operations fs_oper = {
    .getattr	= fs_getattr,
    .readdir	= fs_readdir,
    .open		= fs_open,
    .read		= fs_read,
};


int main(int argc, char *argv[])
{
	readCSVFile();
    return fuse_main(argc, argv, &fs_oper, NULL);
}
