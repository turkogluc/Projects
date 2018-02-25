#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>

#define SEMKEY 8
int sem_id;

//increment operation
void sem_signal(int semid,int val){
	struct sembuf semaphore;
	semaphore.sem_num=0;
	semaphore.sem_op=val;
	semaphore.sem_flg=1;
	semop(semid,&semaphore,1);
}

// decrement operation
void sem_wait(int semid,int val){
	struct sembuf semaphore;
	semaphore.sem_num=0;
	semaphore.sem_op=(-1*val);
	semaphore.sem_flg=1;
	semop(semid,&semaphore,1);
}

// signal handling function
void mysignalfunc(int signum){
	printf("Received signal %d\n",signum);
}

void mysigset(int num){
	struct sigaction mysig;
	mysig.sa_handler = (void *) mysignalfunc;
	mysig.sa_flags    = 0 ;
	sigaction(num,&mysig,NULL);
}

int main(void){

	mysigset(12);
	int f=1,i,children[10];
	
	for(i=0;i<10;i++){
		if(f>0)
			f=fork();
		if(f==0)
			break;
		else
			children[i] = f;	
	}
	if(f>0){
		// In parernt process
		sem_id = semget(SEMKEY,1,IPC_CREAT|0700);
		semctl(sem_id,0,SETVAL,0);
		sleep(1);
		for(i=0;i<10;i++){
			kill(children[i],12);		
		}
		sem_wait(sem_id,10);
		printf("ALL CHILDREN HAS FINISHED....\n");
		semctl(sem_id,0,IPC_RMID,0);
		exit(0);
	}
	else {
		//In child process
		pause();

		sem_id = semget(SEMKEY,1,0);
		printf("I am CHILD creaed %d. AND My PID = %d \n",i,getpid());
		printf("Semaphore value = %d \n",semctl(sem_id,0,GETVAL,0));
		sem_signal(sem_id,1);
	}

	return 0;
}
