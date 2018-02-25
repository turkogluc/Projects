#define _GNU_SOURCE
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KEYSHM2 90
#define KEYSHM 110
#define KEY1 120
#define KEY2 130
#define KEY3 140
#define KEY4 150
#define KEY5 160

int barrier ;
int sem1;
int sem2;
int mutex;
int mutex2;
int *sem_array;
int c;
int *counter ;
int m,n,i,f,shmid;

char *v;

// semaphore functions

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


void yayinla(char *text,int num){
	char haber[40];
	printf("Haber kaynagı %d => '%s' olarak yayınlandı \n",num,text);
	strcpy(v,text);
}

void haberi_oku(int num){
	printf("\tAbone %d Okudu\n",num);

}

int main(){

	counter = NULL;

	printf("Haber kaynağı prosesi sayısını giriniz:");
	scanf("%d",&m);
	printf("Abone proses sayısını giriniz:");
	scanf("%d",&n);

	v = NULL ;

	// shared memory
	shmid = shmget(KEYSHM, sizeof(char)*20, 0700|IPC_CREAT);
	v = (char *)shmat(shmid,0,0);  	/* attach segment to pointer (data space) */

	c = shmget(KEYSHM2, sizeof(int), 0700|IPC_CREAT);
	counter = (int *)shmat(c,0,0);
	*counter = 0;
	

	// semaphore
	barrier = semget(KEY1, 1, 0700|IPC_CREAT);
	semctl(barrier, 0, SETVAL, 0);

	// semaphore
	sem1 = semget(KEY2, 1, 0700|IPC_CREAT);
	semctl(sem1, 0, SETVAL, 0);

	// mutex
	mutex = semget(KEY3, 1, 0700|IPC_CREAT);
	semctl(mutex, 0, SETVAL, 1);

	mutex2 = semget(KEY5, 1, 0700|IPC_CREAT);
	semctl(mutex2, 0, SETVAL, 1);

	sem2 = semget(KEY4, 1, 0700|IPC_CREAT);
	semctl(sem2, 0, SETVAL, 0);

	sem_array = (int *)malloc(sizeof(int)*n);
	int j;
	for(j=0;j<n;j++){
		sem_array[j] = semget(j+1,1,0700|IPC_CREAT);
		semctl(sem_array[j],0,SETVAL,0); // mutex_appr[] semaphore value = 0
	}

	printf("\nHaberler, abonelere dağıtılmaya başlıyor! \n\n");

	for(i=0;i<(m+n);i++){
		f=fork();
		if(f==-1){
			printf("FORK error .. \n");
			exit(1);		
		}
		if(f==0)
			break;
		
	}


//	printf("geldim %d \n",getpid());

	if(f == 0){

		barrier = semget(KEY1, 1, 0700|IPC_CREAT);
		sem1 = semget(KEY2, 1, 0700|IPC_CREAT);
		mutex = semget(KEY3, 1, 0700|IPC_CREAT);
		sem2 = semget(KEY4, 1, 0700|IPC_CREAT);
		mutex2 = semget(KEY5, 1, 0700|IPC_CREAT);
		counter = (int *)shmat(c,0,0);
		


		
		
		for(j=0;j<n;j++){
			sem_array[j] = semget(j+1,1,0700|IPC_CREAT);
		}


		if(i<m){
//			printf("+++ %d kaynakta\n",i);
			
			char text[20] = " haber" ;
			Wait(mutex);			
				yayinla(text,i);
				for(j=0;j<n;j++){
					Signal(sem_array[j]);
				}
				Wait(sem2);
				Signal(barrier);
				
			Signal(mutex);
//	printf("### kaynak %d cıkıyor\n",i);
			//exit(0);
		}else{  
			// abonede
//			printf("+++ %d abone de \n",i-m);
			int z;
			for(z=0;z<m;z++){
				Wait(sem_array[i-m]);
				haberi_oku(i-m);
				Wait(mutex2);


					*counter = (*counter) + 1;
//printf("+++ counter = %d  \n",(*counter));
					if( *counter == n){
//printf("+++ Sem2 sinyallendi\n");
						*counter = 0;
						Signal(sem2);
					}
				Signal(mutex2);
				//printf("\tAbone %d haber okumak istedi ama yeni haber yok, bekliyor.\n",i-m);
			}

//printf("### abone %d cıkıyor\n",i-m);
			//exit(0);
		}
		
	}

	if(f != 0){
		sem_wait(barrier,m);
		printf("Haberler, abonelere dağıtıldı.\n");
	}
return 0;
}
