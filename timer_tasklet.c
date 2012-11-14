#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

#define PROC_FILENAME "timer_tasklet_proc"
#define PARAM_LEN 5

MODULE_LICENSE("Dual BSD/GPL");

static char param_s[PARAM_LEN];
static struct proc_dir_entry *proc_entry;

struct timer_tasklet {
	int delay;
	struct timer_list timer;
	struct tasklet_struct tasklet;
	wait_queue_head_t wait;
};

static ssize_t read_proc(char *buf, char **start, off_t offset, size_t count, int *eof, void *data)
{
	int len = 0;
	
	len = sprintf(buf, "%s\n", param_s);
	return len;
}

static ssize_t write_proc(struct file *file, const char *buf, size_t count, void *data)
{
	if(count > PARAM_LEN)
		count = PARAM_LEN;
		
	if(!copy_from_user(param_s, buf, count)) {
		param_s[count] = '\0';
	}
	else {
		return -EFAULT;
	}

	return count;
}

static int __init tt_init(void)
{
	proc_entry = create_proc_entry(PROC_FILENAME, 0666, NULL);
	if(!proc_entry) {
		printk(KERN_ERR "Error creating /proc entry!");
		return -ENOMEM;
	}
	proc_entry->read_proc = (read_proc_t *)read_proc;
	proc_entry->write_proc = (write_proc_t *)write_proc;
	
	return 0;
}

static void __exit tt_exit(void)
{
	remove_proc_entry(PROC_FILENAME, NULL);
}

module_init(tt_init);
module_exit(tt_exit);
