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
	printf("Received signal %d ...",signum);
	printf("My PID = %d \n",getpid());
}

void mysigset(int num){
	struct sigaction mysig;
	mysig.sa_handler = (void *) mysignalfunc;
	mysig.sa_flags    = 0 ;
	sigaction(num,&mysig,NULL);
}

int main(void){
	// Create a new CHILD Process	
	int f=fork();
	mysigset(10);
	if(f>0){
		sem_id = semget(SEMKEY,1,IPC_CREAT|0700);
		semctl(sem_id,0,SETVAL,0);
printf("Babaya girildi PID = %d \n",getpid());
		// wait awhile
		sleep(3);
		// send the signal 12 to child
printf("Babadan kill \n");
		kill(f,10);
		// wait for semaphore
printf("Baba semaphorda bekliyor \n");
		sem_wait(sem_id,1);
printf("Baba cıkıyor \n");
		// remove semaphore variable
		semctl(sem_id,0,IPC_RMID,0);
		//exit(0);	
	}
	

	if(f==0){
		// IN CHILD PROCESS WAIT
printf("  Cocuga girildi.Pauseda bekliyor PID = %d \n",getpid());
		pause();
printf("  Cocuk semaphore'u sinyalledi \n");
		sem_id = semget(SEMKEY,1,0);
		sem_signal(sem_id,1);

	}
	printf("Bitti \n");
	return 0;
}
