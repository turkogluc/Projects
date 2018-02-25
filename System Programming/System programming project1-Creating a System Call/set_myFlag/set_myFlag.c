#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uidgid.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cred.h>
#include <linux/errno.h>

asmlinkage long sys_set_myFlag(pid_t pid,int flag){
	
	struct task_struct *t;
	if(get_current_user()->uid!=0){	
	printk(KERN_ALERT "######### sys_set_myflag: root problem");
        return -EACCES;   
	}
    t = find_task_by_vpid(pid);
    if(t == NULL){
		printk(KERN_ALERT "######### sys_set_myflag: null problem");
        return -ESRCH;
    }
    if(flag == 0 || flag == 1){
	    t->myFlag = flag;
		printk(KERN_ALERT "######### sysmycall , myflag = %d",t->myFlag);
	}else{
		return -EBADMSG;
	}
    return 0;	
}
