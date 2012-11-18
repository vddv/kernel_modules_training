/* Module for playing with:
 * - Timers
 * - Tasklets
 * - Queues
 * - Threads
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

MODULE_LICENSE("Dual BSD/GPL");

#define DELAY_SECONDS	5

struct tt_data {
	int magic_var;
	int delay;
	struct timer_list timer;
	struct tasklet_struct tasklet;
	struct workqueue_struct workqueue;	
};

struct tt_data ctx = {
	.magic_var = 0,
	.delay = DELAY_SECONDS * HZ,
};

static void on_timer(unsigned long arg)
{
	struct tt_data *data = (struct tt_data *)arg;
	unsigned long j = jiffies;

	// Sheduling tasklet *before* timer output
	tasklet_schedule(&data->tasklet);

	++data->magic_var;
	printk("(^_^) **TIMER** jiffies: %li, magic_var: %3d%s\n", 
			j, data->magic_var, in_interrupt() ? ", in_irq" : "");
	
	data->timer.expires += data->delay;
	add_timer(&data->timer);
}

static void run_timer(void)
{
	struct tt_data *data = &ctx;
	unsigned long j = jiffies;
	
	init_timer(&data->timer);

	data->timer.data = (unsigned long)data;
	data->timer.function = on_timer;
	data->timer.expires = j + data->delay;

	add_timer(&data->timer);
}

static void stop_timer(void)
{
	struct tt_data *data = &ctx;
	
	del_timer_sync(&data->timer);
}

static void run_tasklet(unsigned long arg)
{
	struct tt_data *data = (struct tt_data *)arg;
	unsigned long j = jiffies;

	++data->magic_var;
	printk("(^_^) *TASKLET* jiffies: %li, magic_var: %3d%s\n", 
			j, data->magic_var, in_interrupt() ? ", in_irq" : "");
}

static void init_tasklet(void)
{
	struct tt_data *data = &ctx;
	
	tasklet_init(&data->tasklet, run_tasklet, (unsigned long)data);
}

static void del_tasklet(void)
{
	struct tt_data *data = &ctx;
	
	tasklet_kill(&data->tasklet);
}

static int __init tt_init(void)
{
	printk(	"(^_^) Init...\n" \
			"      HZ = %d\n", HZ);
	
	init_tasklet();
	run_timer();
		
	return 0;
}

static void __exit tt_exit(void)
{
	stop_timer();
	del_tasklet();
}

module_init(tt_init);
module_exit(tt_exit);
