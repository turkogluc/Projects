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

#define SEMKEY1 10
#define SEMKEY2 20
#define LOCKKEY 30
#define KEYSHM 40

/* ------------------------------- Necessary Data to WEC Process -------------------------------*/

#define KEY1 100	// keys for semaphores of WEC Process
#define KEY2 101
#define KEY3 102
#define KEY4 103

// semaphore variables and shared variables of WEC Process
int T;				// member and question number
int readCounter; 	// flag for checking that whether all threads read file or not
int mutex_read;		// for readCounter update, (make it safe)
int *mutex_appr;	// mutex for approval variable 
int syn;			// semaphore for sycnorization, (wait until all threats to create question)
int barrier;

int finishCounter;

typedef struct q{
	char text[30];
	int approval;
}question; 

question *qSheet;	// dynamic array

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
	//Sem1 = semget(SEMKEY1, 1, 0700|IPC_CREAT); 
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

				finishCounter += 1 ;
				printf("### finishCounter = %d\n",finishCounter);

				
			}
		Signal(mutex_appr[x]);
		}
	}
	


	fclose(fptr);
	pthread_exit(NULL);
	printf("###bitiyor\n");
}

/* -----------------------------**  End Of Necessary Data to WEC Process **----------------------------- */

/* -----------------------------**  Registrar and Interviewer process **-------------------------------- */
typedef struct application{
	int exam_score;
	int interview_score;
	int total_score;
}ApplicationForm;


void mysignal(int signum){
	//printf("Received signal with num = %d.\n",signum);  // Do Nothing //
}

void mysigset(int num){
	struct sigaction mysigaction;
	mysigaction.sa_handler=(void *)mysignal;
	mysigaction.sa_flags=0;
	sigaction(num,&mysigaction,NULL);
}



/* ------------------------------------**   Main Function  **---------------------------------- */

	
int main(int argc,char *argv[]){
	
	mysigset(12);
	int M=1,I=1;
	T = 3;
int Sem1 = 0 , Sem2 = 0 , Lock = 0 ; // 2 semaphore and 1 mutual ex
	int shmid = 0;		// shared memory id
	ApplicationForm *AppBox = NULL ;
	int f; 		// return value for fork
	int child[2]; 	// child process ids
	int i, myOrder = 0;
	int SIZE = sizeof(ApplicationForm)*3 ;
	
	
	/* Creating 2 child Processes */

	for(i=0;i<2;i++){
		f=fork();
		if(f==-1){
			printf("FORK error .. \n");
			exit(1);		
		}
		if(f==0)
			break;
		child[i] = f ;	
	}

	if (f != 0){
		
		/* For rand function, time(NULL) gives same seed because they run at same time
		   So for different seeds , I gave it PID as seed
		*/

		srand(getpid());	

		/* Creating Semaphore and Mutex */

		Sem1 = semget(SEMKEY1, 1, 0700|IPC_CREAT);
		semctl(Sem1, 0, SETVAL, 0);	
		// sem1 , initial value = 0

		/*Sem2 = semget(SEMKEY2, 1, 0700|IPC_CREAT);
		semctl(Sem2, 0, SETVAL, 0);	
		// sem2 , initial value = 0 */

		Lock = semget(LOCKKEY, 1, 0700|IPC_CREAT);
		semctl(Lock, 0, SETVAL, 1);
		// lock , initial value = 1
	
		/*  Create the segment: 
		 *  There are 3 applicant so 3 application form in array 
    		 *  SIZE = 3 * sizeof(ApplicationForm)  assigned in main function
		 */
		
		shmid = shmget(KEYSHM, SIZE, 0700|IPC_CREAT);
		/* attach segment to pointer (data space) */
		AppBox = (ApplicationForm *)shmat(shmid,0,0);
		printf("The Registrar started taking applications.\n");

		int k;
		for(k=0;k<3;k++){
			AppBox[k].exam_score = 0;
			AppBox[k].interview_score = 0;
			AppBox[k].total_score = 0;
			sleep(3);		
		}

		printf("The Registrar finished taking applications.\n");
		//kill(child[0],12); // Send Signal To WEC process
		//Wait(Sem1); // Wait Here Until WEC process completes its task
		

		/* Second Task of Registrar Process is making exam */
		printf("The Registrar started the written exam.\n");
		for(k=0;k<3;k++){
			// write scores to each app. form and sleep M sec.
			AppBox[k].exam_score = rand()%50;
			sleep(M);		
		}
		printf("Written exam score of applicant 1 is %d\n",AppBox[0].exam_score);		
		printf("Written exam score of applicant 2 is %d\n",AppBox[1].exam_score);
		printf("Written exam score of applicant 3 is %d\n",AppBox[2].exam_score);
		
		shmdt(AppBox); // # DETACH	
		printf("The Registrar finished the written exam.\n");

		kill(child[1],12); // Send Signal To Interviewer process
		Wait(Sem1); // Wait Here Until Interviewer ps. completes its task
		
		printf("The Registrar started calculating total scores.\n");
		AppBox = (ApplicationForm *)shmat(shmid,0,0); // # ATACH

		for(k=0;k<3;k++){
			AppBox[k].total_score = AppBox[k].exam_score + AppBox[k].interview_score ;
			sleep(3);
		}

		printf("Total score of applicant 1 is %d\n",AppBox[0].total_score);		
		printf("Total score of applicant 2 is %d\n",AppBox[1].total_score);
		printf("Total score of applicant 3 is %d\n",AppBox[2].total_score);

		shmdt(AppBox); // DETACH
		printf("The Registrar finished calculating total scores.\n");

	}
	else{  	// child
		srand(getpid()); 	// for random function
		myOrder = i; // to determine which child

		if(myOrder == 0){ // means that this is WEC process
			pause(); // wait here
			
			/*  WEC PROCESS PREPARES EXAM QUESTIONS */
			printf("The Written Exams Committee started preparing questions.\n");

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
			
			/*
			barrier = semget(KEY4,1,0700|IPC_CREAT);
			semctl(barrier,0,SETVAL,0); // barrier semaphore value = 0
			*/

			qSheet = (question *)malloc(sizeof(question)*(T+1));
	
			pthread_t th[T+1];
			long i;
	
			for(i=1;i<(T+1);i++){
				pthread_create(&th[i],NULL,tFunc,(void *)i);
			}
	
			

// ** Buraya bir semaphore (bariyer) daha koymak lazım . Bütün threat'ler bitirdiğinde aşağı inmeye izin verilecek **/

			
			/* ************* END OF WEC PROCESS *************** */

			printf("The Written Exams Committee finished preparing questions.\n");
			
			if(finishCounter == T){
				printf("### Buraya girdi \n");
			// # Wait()
				Signal(Sem1);
			
			}


			
			// remove semaphores
			semctl(mutex_read,0,IPC_RMID,0) ;
			semctl(syn,0,IPC_RMID,0) ;
			for(i=1;i<T+1;i++){
				semctl(mutex_appr[i],0,IPC_RMID,0) ;		
			}

			pthread_exit(NULL);
			exit(1);
			
		}else if(myOrder == 1){ // means that this is InterViewer process
			pause(); // wait here
			
			printf("The Interviewer started interviews.\n");
			
			shmid = shmget(KEYSHM, SIZE, 0700|IPC_CREAT);
			/* attach segment to pointer (data space) */
			AppBox = (ApplicationForm *)shmat(shmid,0,0);

			int j;			
			for(j=0;j<3;j++){
				// write scores to each app. form and sleep M sec.
				AppBox[j].interview_score = (rand())%50;
				sleep(1);		
			}	

			printf("Interview score of applicant 1 is %d\n",AppBox[0].interview_score);		
			printf("Interview score of applicant 2 is %d\n",AppBox[1].interview_score);
			printf("Interview score of applicant 3 is %d\n",AppBox[2].interview_score);

			shmdt(AppBox); // # DETACH
			printf("The Interviewer finished interviews.\n");
			Signal(Sem1);
			exit(0);
		}
	}	

return 0;
}
