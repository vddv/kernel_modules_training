#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>


#define THREAD_AMOUNT   42
#define PROC_FILENAME   "megathreads"

struct threads {
    ulong total_itr;
    atomic_t total_itr_atm;
    int itr[THREAD_AMOUNT];
    struct semaphore mlock;
    int id[THREAD_AMOUNT];
    struct task_struct *task[THREAD_AMOUNT];
    int sleep_time[THREAD_AMOUNT];
    struct proc_dir_entry *proc;
};

static struct threads *dev;


// longlock func
static inline int calculate_total(ulong *d)
{
    if (down_interruptible(&dev->mlock))
        return -ERESTARTSYS;
    
    *d = 0;
    for (int i = 0; i < THREAD_AMOUNT; i++) {
        *d += dev->itr[i];
    }
    
    up(&dev->mlock);
    return 0;
}

static ssize_t read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	
    len = sprintf(buf, "TOTAL ITERATIONS COMPARE: (calc_ctr) %ld, (atomic_ctr) %d\n", 
            dev->total_itr, atomic_read(&dev->total_itr_atm));
    len += sprintf(buf+len, "THREAD\tITERATIONS\n");
    for (int i = 0; i < THREAD_AMOUNT; i++) {
        len += sprintf(buf+len, "%6d\t%10d\n", i, dev->itr[i]);
    }
	return len;
}

static int task_routine(void *data)
{
    int i = *(int *)data;
    
    while (!kthread_should_stop()) 
    {
        ++dev->itr[i];
        atomic_inc(&dev->total_itr_atm);
        while ( calculate_total(&dev->total_itr) )
            ;
            
        /*
        printk(KERN_INFO "THREAD_%03d, PID:%5d, CPU:%2d\n", 
                i, dev->task[i]->pid, dev->task[i]->on_cpu);
        */
        
        msleep(dev->sleep_time[i]); // msleep_interruptible?
        
        //schedule();
    }

    printk(KERN_INFO "THREAD_%03d STOPPED.", i);
    return 0;
}

static int __init threads_init(void)
{
    dev = kmalloc(sizeof(struct threads), GFP_KERNEL);
    if (dev == NULL) {
        printk(KERN_ERR "Out of memory!\n");
        return -ENOMEM;
	}
	memset(dev, 0, sizeof(*dev));
    
    sema_init(&dev->mlock, 1); // init_MUTEX
    
    dev->proc = create_proc_read_entry(PROC_FILENAME, 0, NULL, read_proc, NULL);
	if(!dev->proc) {
		printk(KERN_ERR "Error creating /proc entry!");
		return -ENOMEM;
	}

    for (int i = 0; i < THREAD_AMOUNT; i++) {
        dev->id[i] = i;
        dev->sleep_time[i] = ((i % 9) + 1) * 9000 / THREAD_AMOUNT; 
                        // magic number is over 9000 (*!*)
        dev->task[i] = kthread_run(&task_routine, &dev->id[i], 
                        "megathread_%03d", i);
    }
    
    return 0;
}

static void __exit threads_exit(void)
{
    for (int i = 0; i < THREAD_AMOUNT; i++) {
        kthread_stop(dev->task[i]);
    }
    remove_proc_entry(PROC_FILENAME, NULL);
    kfree(dev);
}

module_init(threads_init);
module_exit(threads_exit);

MODULE_LICENSE("Dual BSD/GPL");
