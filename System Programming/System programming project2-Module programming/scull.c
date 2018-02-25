/* Cemal turkoglu - Rıdvan Sirma */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/cred.h>
#include <linux/string.h>
#include <linux/sched.h>

#include <asm/switch_to.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */

#include "scull_ioctl.h"

#define SCULL_MAJOR 0
#define SCULL_NR_DEVS 4
#define SCULL_QUANTUM 4000
#define SCULL_QSET 1000
#define LIM_TO 20
#define LIM_FROM 20
#define LIM_MSG 100

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;
int read_mode = 0 ; // 0 : EXCLUDE_READ (DEFAULT)
					// 1 : INCLUDE_READ
int mailbox_size = 3 ;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev {
    char **data;
    int quantum;
    int qset;
    unsigned long size;
    struct semaphore sem;
    struct cdev cdev;
};

struct scull_dev *scull_devices;

typedef struct Node{
	int read_flag;
	char to[LIM_TO];
	char from[LIM_FROM];
	char message[LIM_MSG];
	struct Node* next;
	
}node;

node *head = NULL;

node *create_node(char to[],char from[], char message[]){
	node *new = (node*)kmalloc(sizeof(node),GFP_KERNEL);
	if(new == NULL){
		printk(KERN_ALERT "Node could not created\n");
		return NULL;
	}
	new->read_flag = 0; // not read yet
	strcpy(new->to,to);
	strcpy(new->from,from);
	strcpy(new->message,message);
	new->next = NULL;
	return new;
}

int add_node(char to[],char from[], char message[]){
		if(head == NULL){
			head = create_node(to,from,message);
			return 2;
		}else{
			node *trv = head;
			while(trv->next != NULL)
				trv = trv->next;
				
			node *new = create_node(to,from,message);
			trv->next = new ;
			return 1;	
		}
		return 0;	
}

int traverse(void){
	
		if(head == NULL){
			printk(KERN_ALERT "$ List is Empyt\n");
			return -1;
		}
		node *trv = head;
		
		if(head->next == NULL){
			//printk(KERN_ALERT "$ READ: %d , TO: %s , FROM : %s , MSG : %s \n",head->read_flag,head->to,head->from,head->message);
			return 1;
		}else{
			
			while(trv->next !=NULL){
				//printk(KERN_ALERT "$ READ: %d , TO: %s , FROM : %s , MSG : %s \n",trv->read_flag,trv->to,trv->from,trv->message);
				trv= trv->next;
			}
			//printk(KERN_ALERT "$ READ: %d , TO: %s , FROM : %s , MSG : %s \n",trv->read_flag,trv->to,trv->from,trv->message);
			return 1;
		}
		
		return 1;
}

int delete_node(node *n){
	
	if(head == NULL){		// if list is empty
		printk(KERN_ALERT "$ Delete error. List is empty \n");
		return -1;
	}
	else if(head == n){		// if head will be deleted
		
		if(head->next == NULL){		// if there is only 1 elemenet
			node *tmp = head;
			kfree(tmp);
			head = NULL;
			return 1;
		}else{						// if there is more than 1 element
			node *tmp = head;
			head = head->next;
			kfree(tmp);
			return 2;
		}

	}
	else{					
		node *tmp = head ;
		while(tmp->next != n && tmp->next != NULL)
			tmp = tmp->next;
		
		if(tmp->next == NULL){
			printk(KERN_ALERT "$ There is no such element in the list to be deleted. \n");
			return 3;
		}else{
			node *del = tmp->next;
			tmp->next = del->next;
			kfree(del);
			return 4;
			
		}
		
	}
	
}

char* find_username(char* uid){
		struct file *f;
		mm_segment_t fs;
		char str[5000];
		loff_t offset;
		offset = 0;
		
		f = filp_open("/etc/passwd",O_RDONLY,0644);
		fs = get_fs();
		set_fs(get_ds());
		
		vfs_read(f,str,5000,&offset);
		set_fs(fs);
		filp_close(f,NULL);
		
		//printk(KERN_ALERT "PASSWD = %s \n",str);
		
		char *token;
		char *username;
		char *userid;
		char *arg = str;

		while(token = strsep(&arg,"\n")){
			
			//printk(KERN_ALERT "TOKEN = %s \n",token);
			username = strsep(&token,":");
			strsep(&token,":");
			userid = strsep(&token,":");
			if(strcmp(userid,uid) == 0){
				return username;		
			}		
		}	
		return "null";
}

char *read_message(char uname[]){
	char *result = " ";
	char *del;

	int size = strlen(result);
	node *tmp = head;
	while(tmp != NULL ){
		if(strcmp(tmp->to,uname)==0){
			
			// in EXCLUDE_READ mode , do not read if a message is already read
			if(read_mode == 0 && tmp->read_flag == 1){
				tmp = tmp->next;
				continue ;
			}
			
			del = (char *)kmalloc(strlen(result)*sizeof(char),GFP_KERNEL);
			strcpy(del,result);
			//del = &del[1];
			
			//printk(KERN_ALERT "C- RESULT = %s\n",result);
			//printk(KERN_ALERT "C- DEL = %s\n",del);
			size += strlen(tmp->from) + strlen(tmp->message) + 2;
			result = (char *)kmalloc(size*sizeof(char),GFP_KERNEL);
			
			//printk(KERN_ALERT "B- RESULT = %s\n",result);
			//printk(KERN_ALERT "B- DEL = %s\n",del);
			strcpy(result,del);
			strcat(result,tmp->from);

			strcat(result,":");
			strcat(result,tmp->message);
			strcat(result,"\n ");
			
			//printk(KERN_ALERT "A- RESULT = %s\n",result);
			//printk(KERN_ALERT "A- DEL = %s\n",del);
			
			kfree(del);
			tmp->read_flag = 1;

		}
		tmp = tmp->next;	
	}
	
	//result[0] = ' ';
	//printk(KERN_ALERT "FINEL - RESULT = %s\n",result);
	
	return result;
	
}

int check_mailbox_size(char username[]){
	int count = 0;
	node *traverse = head;
	while(traverse != NULL){
		if(strcmp(traverse->to,username) == 0 && traverse->read_flag == 0){
			count=count+1;
			printk(KERN_ALERT "COUNT = %d\n",count);
		}
		traverse = traverse->next ;	
	}
	if(count >= mailbox_size ){
		printk(KERN_ALERT "Mailbox is full \n");
		return 0; 	// error
	}
	printk(KERN_ALERT "Mailbox is not full \n");
	return 1;		// OK.
}



int scull_trim(struct scull_dev *dev)
{
    int i;

    if (dev->data) {
        for (i = 0; i < dev->qset; i++) {
            if (dev->data[i])
                kfree(dev->data[i]);
        }
        kfree(dev->data);
    }
    dev->data = NULL;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->size = 0;
    return 0;
}


int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_dev *dev;

	filp->f_flags = O_APPEND;		// to overwrite the file

    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev;
    
    

    /* trim the device if open was write-only */
    if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
        scull_trim(dev);
        up(&dev->sem);
    }
    return 0;
}


int scull_release(struct inode *inode, struct file *filp)
{
    return 0;
}


ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos)
{
	//printk(KERN_ALERT "A COUNT = %d , \n",count);
    struct scull_dev *dev = filp->private_data;
    int quantum = dev->quantum;
    int s_pos, q_pos;
    ssize_t retval = 0;
    
    printk(KERN_ALERT "R-A- COUNT = %d , quantum = %d , dev->size = %d , f_pos = %d \n",count,quantum,dev->size,*f_pos);

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    if (*f_pos >= dev->size)
        goto out;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;

	printk(KERN_ALERT "R-B- COUNT = %d , quantum = %d , dev->size = %d , f_pos = %d \n",count,quantum,dev->size,*f_pos);

    s_pos = (long) *f_pos / quantum;
    q_pos = (long) *f_pos % quantum;

    if (dev->data == NULL || ! dev->data[s_pos])
        goto out;

    /* read only up to the end of this quantum */
    if (count > quantum - q_pos)
        count = quantum - q_pos;
    
       
     
    char uid[LIM_FROM];
	sprintf(uid,"%d",current_uid());
	
	char *username;
	int from_size = strlen(uid);
	username = (char *) kmalloc(from_size * sizeof(char),GFP_KERNEL);
	strcpy(username,find_username(uid));  		// username -> will be current username
	
	char *messages = read_message(username);
	
	count = strlen(messages);
	if(dev->size > count){
		printk(KERN_ALERT "R-XXX- COUNT = %d , quantum = %d , dev->size = %d , f_pos = %d \n",count,quantum,dev->size,*f_pos); 
		dev->size = count;
	}
	printk(KERN_ALERT "MESSAGE = %s , strlen = %d , count = %d \n",messages,strlen(messages),count);
	  
    // dev->data[s_pos] + q_pos = messages ;

    if (copy_to_user(buf, messages , count)) {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval = count;
    
    printk(KERN_ALERT "R-C- COUNT = %d , quantum = %d , dev->size = %d , f_pos = %d \n",count,quantum,dev->size,*f_pos); 

  out:
    up(&dev->sem);
    return retval;
}


ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    int quantum = dev->quantum, qset = dev->qset;
    int s_pos, q_pos;
    ssize_t retval = -ENOMEM;
    
    //#printk(KERN_ALERT "W-A- COUNT = %d , dev->size = %d , f_pos = %d \n",count,dev->size,*f_pos);

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (*f_pos >= quantum * qset) {
        retval = 0;
        goto out;
    }

    s_pos = (long) *f_pos / quantum;
    q_pos = (long) *f_pos % quantum;

    if (!dev->data) {
        dev->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
        if (!dev->data)
            goto out;
        memset(dev->data, 0, qset * sizeof(char *));
    }
    if (!dev->data[s_pos]) {
        dev->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
        if (!dev->data[s_pos])
            goto out;
    }
    /* write only up to the end of this quantum */
    if (count > quantum - q_pos)
        count = quantum - q_pos;
        
    //#printk(KERN_ALERT "W-B- COUNT = %d , dev->size = %d , f_pos = %d \n",count,dev->size,*f_pos);    
        
     			
	//printk(KERN_ALERT "hello\n");  
	char *to,*from,*message;
		
	char temp[LIM_FROM];
	char *temp2 ;
	char *temp3;
		
	sprintf(temp,"%d",current_uid());
		
	int from_size = strlen(temp);
	from = (char *) kmalloc(from_size * sizeof(char),GFP_KERNEL);
	strcpy(from,find_username(temp));
		

	char * bufTemp = (char *)kmalloc(strlen(buf)*sizeof(char),GFP_KERNEL);
	strcpy(bufTemp,buf);
	temp2 = strsep(&bufTemp," ");  // temp2 => @bob
	int to_size = strlen(temp2)-1 ;  // to get rid of @ charachter from the beginning
	to = (char *) kmalloc(to_size * sizeof(char),GFP_KERNEL);
	to = &temp2[1];		// to => bob

	temp3 = strsep(&bufTemp,"\n");
	int size_msg = strlen(temp3);
	message = (char *)kmalloc(size_msg * sizeof(char),GFP_KERNEL);
	strcpy(message,temp3);
		
	//printk(KERN_ALERT "TO : %s , FROM : %s , MSG : %s \n",to,from,message);
	
	// to check mailbox size 	
	if(check_mailbox_size(to) == 0){
		printk(KERN_ALERT "MAIL BOX IS FULL \n");
		retval = -EFAULT;
        goto out;
	}else{ 	  
		add_node(to,from,message);
		kfree(message);
	}

    if (copy_from_user(dev->data[s_pos] + q_pos, buf, count)) {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval = count;

    /* update the size */
    if (dev->size < *f_pos)
        dev->size = *f_pos;
        
    //#printk(KERN_ALERT "W-C- COUNT = %d , dev->size = %d , f_pos = %d \n",count,dev->size,*f_pos);    

  out:
    up(&dev->sem);
    return retval;
}

long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

	int err = 0, tmp;
	int retval = 0;

	char buf[LIM_TO];
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch(cmd) {
	  case SCULL_IOCRESET:
		scull_quantum = SCULL_QUANTUM;
		scull_qset = SCULL_QSET;
		break;

	  case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_quantum = arg;
		break;

	  case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
		retval = __put_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
		return scull_quantum;

	  case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		retval = __get_user(scull_quantum, (int __user *)arg);
		if (retval == 0)
			retval = __put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		scull_quantum = arg;
		return tmp;

	  case SCULL_IOCSQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCTQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_qset = arg;
		break;

	  case SCULL_IOCGQSET:
		retval = __put_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCQQSET:
		return scull_qset;

	  case SCULL_IOCXQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		retval = __get_user(scull_qset, (int __user *)arg);
		if (retval == 0)
			retval = put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		scull_qset = arg;
		return tmp;
		
	  case READ_MODE:
		if (! capable (CAP_SYS_ADMIN)){
			printk(KERN_ALERT "ONLY SUPERUSER CAN CHANGE THE READ_MODE \n");
			return -EPERM;
		}	
		if(arg == 0 || arg ==1){
			read_mode = arg;
			
		}else{
			return -EBADMSG;
		}
	  break;
	  	
	  case DELETE_MESSAGES:
	    if (! capable (CAP_SYS_ADMIN)){
			printk(KERN_ALERT "ONLY SUPERUSER CAN DELETE MESSAGES \n");
			return -EPERM;
		}	
		copy_from_user(&buf,(char *)arg,20*sizeof(char));
		// buf username ismi oldu.. loop içinde o mesajlar silinmeli
		node *temp = head ;
		while(temp->next != NULL){			// traverse the list
			if(strcmp(temp->to,buf) == 0){	// find messages to be deleted for the user(->buf in icinde)
				delete_node(temp);
			}
			temp = temp->next;
		}
		if(strcmp(temp->to,buf) == 0){		// for last element
			delete_node(temp);
		}
	
	  break;	

	  default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}
	return retval;
}


loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *dev = filp->private_data;
    loff_t newpos;

    switch(whence) {
        case 0: /* SEEK_SET */
            newpos = off;
            break;

        case 1: /* SEEK_CUR */
            newpos = filp->f_pos + off;
            break;

        case 2: /* SEEK_END */
            newpos = dev->size + off;
            break;

        default: /* can't happen */
            return -EINVAL;
    }
    if (newpos < 0)
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}


struct file_operations scull_fops = {
    .owner =    THIS_MODULE,
    .llseek =   scull_llseek,
    .read =     scull_read,
    .write =    scull_write,
    .unlocked_ioctl =  scull_ioctl,
    .open =     scull_open,
    .release =  scull_release,
};


void scull_cleanup_module(void)
{
	// free all nodes
	if(head == NULL){		// if list is empty
		printk(KERN_ALERT "$ Delete error. List is empty \n");
	}
	node *trv = head ;
	while(trv != NULL){
		node *tmp = trv ;
		trv = trv->next ;
		kfree(tmp);
	}

    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);

    if (scull_devices) {
        for (i = 0; i < scull_nr_devs; i++) {
            scull_trim(scull_devices + i);
            cdev_del(&scull_devices[i].cdev);
        }
    kfree(scull_devices);
    }

    unregister_chrdev_region(devno, scull_nr_devs);
}


int scull_init_module(void)
{
    int result, i;
    int err;
    dev_t devno = 0;
    struct scull_dev *dev;

    if (scull_major) {
        devno = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(devno, scull_nr_devs, "scull");
    } else {
        result = alloc_chrdev_region(&devno, scull_minor, scull_nr_devs,
                                     "scull");
        scull_major = MAJOR(devno);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }

    scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev),
                            GFP_KERNEL);
    if (!scull_devices) {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

    /* Initialize each device. */
    for (i = 0; i < scull_nr_devs; i++) {
        dev = &scull_devices[i];
        dev->quantum = scull_quantum;
        dev->qset = scull_qset;
        sema_init(&dev->sem,1);
        devno = MKDEV(scull_major, scull_minor + i);
        cdev_init(&dev->cdev, &scull_fops);
        dev->cdev.owner = THIS_MODULE;
        dev->cdev.ops = &scull_fops;
        err = cdev_add(&dev->cdev, devno, 1);
        if (err)
            printk(KERN_NOTICE "Error %d adding scull%d", err, i);
    }

    return 0; /* succeed */

  fail:
    scull_cleanup_module();
    return result;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
