/*
  *	compile :	gcc test2.c -o test2
  *    usage     :	./test2.c [ FLAG VALUE ] [ NICE VALUE ]
  *    
  *	Program takes flag parameter to set myFlag value of  itself.
  *	After it makes a fork call to create a child. Child process waits with an getchar()  or loop, to be not killed before parent
  * 	Parent dies and if it has 1 as myFlag value and has a nice value greater than 10 , it kills also child process
  *	To check ; we sent flag 0 and child process is not killed , it waits.
  *			      we sent flag 1 and when parent is going to die it kills the child first.
  */



#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#define NR_SETMYFLAG 356



int main(int argc, char *argv[]){
	
	int f=1;
	int pid=getpid();
	int flag=atoi(argv[1]);
	int nice=atoi(argv[2]);
	printf("parent pid = %d\n",pid);
	setpriority(PRIO_PROCESS,getpid(),nice);
	long y = syscall(NR_SETMYFLAG,pid,flag);
	if(y!=0){
		printf("%s \n", strerror(errno));
	}
	f=fork();
	if(f==0){
		printf("child pid = %d\n",getpid());
		//getchar();
		while(1);
		printf("child is going to die \n");
	}else if(f>0){
		sleep(1);
		printf("parent is going to die %d\n",getpid());
		exit(0);
		
	}	

	return 0;
}
