/* Module for playing with:
 * - Timers
 * - Tasklets
 * - Queues (?)
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define DELAY_SECONDS	5

static struct workqueue_struct *wq = NULL;
static void workqueue_fn(void *);
static DECLARE_DELAYED_WORK(tt_work, workqueue_fn);  // ?

struct tt_data {
	int magic_var;
	int delay;
	int wq_delay;
	struct timer_list timer;
	struct tasklet_struct tasklet;
    
};

struct tt_data ctx = {
	.magic_var = 0,
	.delay = DELAY_SECONDS * HZ,
	.wq_delay = (DELAY_SECONDS * HZ) - (HZ / 5),
};

static void workqueue_fn(void *arg) // how to pass arg in workqueue?
{
	struct tt_data *data = &ctx;
	unsigned long j = jiffies;

	++data->magic_var;
	printk("(^_^) **QUEUE** jiffies: %li, magic_var: %3d%s\n",
			j, data->magic_var, in_interrupt() ? ", in_irq" : "");
			
	queue_delayed_work(wq, &tt_work, data->wq_delay);
}

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

static void init_workqueue(void)
{
	struct tt_data *data = &ctx;

	if (!wq)
		wq = create_singlethread_workqueue("tt_work");
	if (wq)
		queue_delayed_work(wq, &tt_work, data->wq_delay);
}

static void cancel_workqueue(void)
{
	if (wq){
		cancel_delayed_work(&tt_work);
		flush_workqueue(wq);
		destroy_workqueue(wq);
	}
}

static int __init tt_init(void)
{
	printk(	"(^_^) Init...\n" \
			"      HZ = %d\n", HZ);

	init_tasklet();
	init_workqueue();
	run_timer();

	return 0;
}

static void __exit tt_exit(void)
{
	stop_timer();
	del_tasklet();
	cancel_workqueue();
}

module_init(tt_init);
module_exit(tt_exit);

MODULE_LICENSE("Dual BSD/GPL");
