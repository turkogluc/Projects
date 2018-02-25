// Cemal Turkoglu
// 150140719

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#define KEY1 100
#define KEY2 101
#define KEY3 102


// semaphore variables and shared variables
int T;				// member and question number
int readCounter; 	// flag for checking that whether all threads read file or not
int mutex_read;		// for readCounter update, (make it safe)
int *mutex_appr;	// mutex for approval variable 
int syn;			// semaphore for sycnorization, (wait until all threats to create question)


typedef struct q{
	char text[30];
	int approval;
}question; 

question *qSheet;	// dynamic array


void sem_signal(int semid,int val){	// to increase val
	struct sembuf semaphore;		// V() function
	semaphore.sem_num = 0;
	semaphore.sem_op = val;
	semaphore.sem_flg = 1;
	semop(semid,&semaphore,1);
}
void sem_wait(int semid,int val){ // to decrease val
	struct sembuf semaphore;	  // P() function
	semaphore.sem_num = 0;
	semaphore.sem_op = (-1*val);
	semaphore.sem_flg = 1;
	semop(semid,&semaphore,1);
}
void Wait(int s){		// to decrease 1
	sem_wait(s,1);
}
void Signal(int s){		// to increase 1
	sem_signal(s,1);
}

// thread function

void *tFunc(void *ptr){
	long i =  (long) ptr;
	
	// read file	
	FILE *fptr = fopen("input.txt","r");
	int memberNum;
	char text[30];
	int n;
	int indexOFquestion;
	while(!feof(fptr)){
		// every thread has an id which is specified by "i"
		// and every thread finds its related question via reading line by line
		fscanf(fptr,"%d %s %d",&memberNum,text,&n);
		if(memberNum == i){  // if this is my question
			
			indexOFquestion = memberNum;	
			//printf("%ld => %d %s %d \n",i,memberNum,text,n);
			strcat(text,"_question");
			strcpy(qSheet[indexOFquestion].text,text); // write the question
			qSheet[indexOFquestion].approval = 0; // initialize approval as zero
			sleep(n); // preparing time
			
		printf("WEC Member %d: A question prepared on %s\n",indexOFquestion,text);
		}
	}
	
	
/////////////////////  end of read from file ///////////////////////	

	Wait(mutex_read); // mutex_begin
		readCounter++ ;
		if(readCounter == T){
			sem_signal(syn,T);	// allow waiting all T threats
		}
	Signal(mutex_read); // mutex_end 
	
//////////////////// read part is finished ///////////////////////


///////////////////  Approval part ////////////////////////////////	
	Wait(syn); 	
	int x;
	for(x=1;x<(T+1);x++){
		if(x != indexOFquestion){ // thread should not check its own question
		// here for all question there is needed a lock mechanism
		// so mutex array is used
		 	
		Wait(mutex_appr[x]);
			sleep(1);
			qSheet[x].approval += 1;
			//printf("%d makes approval[%d] = %d \n",indexOFquestion,x,qSheet[x].approval);
			if(qSheet[x].approval == (T-1)){
				// if all other members approved the question 
				// then that is ready
				printf("WEC Member %d: Question %d is ready..\n",indexOFquestion,x);
				
			}
		Signal(mutex_appr[x]);
		}
	}

	fclose(fptr);
	pthread_exit(NULL);
}

// main function

int main(int argc, char **argv){
	printf("Program cannot take parameter as argument\n");
	printf("Please write the value of T : \n");
	scanf("%d",&T);
	
	readCounter = 0;
	
	mutex_read = semget(KEY1,1,0700|IPC_CREAT);
	semctl(mutex_read,0,SETVAL,1); // mutex_read semaphore value = 1
	
	mutex_appr = (int *)malloc(sizeof(int)*(T+1));
	int j;
	for(j=1;j<(T+1);j++){
		mutex_appr[j] = semget(j,1,0700|IPC_CREAT);
	    semctl(mutex_appr[j],0,SETVAL,1); // mutex_appr semaphore value = 1
	}

	syn = semget(KEY2,1,0700|IPC_CREAT);
	semctl(syn,0,SETVAL,0); // syn semaphore value = 0
	

	qSheet = (question *)malloc(sizeof(question)*(T+1));
	
	pthread_t th[T+1];
	long i;
	
	for(i=1;i<(T+1);i++){
		pthread_create(&th[i],NULL,tFunc,(void *)i);
	}
	
	pthread_exit(NULL);
	// remove semaphores
	semctl(mutex_read,0,IPC_RMID,0) ;
	semctl(syn,0,IPC_RMID,0) ;
	for(i=1;i<T+1;i++){
		semctl(mutex_appr[i],0,IPC_RMID,0) ;		
	}

	
	return 0;
}
