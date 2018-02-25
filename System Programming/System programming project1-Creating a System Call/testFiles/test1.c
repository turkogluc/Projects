/*
  *	Compile : gcc test1.c -o test1
  *	Usage :  ./test1 [ PID ] [ FLAG VALUE ]
  *	Program sets the myFlag value of process with given Pid
  *	If caller process doesn't have root permissions,  proper error will be printed
  *	If there is no process with given pid,  proper error will be printed
  *	Flag can be only 1 or 0, for any other values,  proper error will be printed
  */  
	


#include <stdio.h>
#include <errno.h>
#include <string.h>
#define NR_SETMYFLAG 356

int main( int argc, char *argv[] )  {
	
	int pid=atoi(argv[1]);
	int flag=atoi(argv[2]);
	long y=syscall(NR_SETMYFLAG,pid,flag);
	if(y!=0){
		printf("%s \n", strerror(errno));
	}
	return 0;
}
