#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>

asmlinkage long sys_set_myFlag(pid_t pid,int flag){
	
	struct task_struct *tsk;
	tsk = find_task_by_vpid(pid);
	tsk->myFlag = flag;
	printk(KERN_ALERT "\n\n # flag has changed , new value = %d",tsk->myFlag);
	
	
	return 0;
	
	
}
