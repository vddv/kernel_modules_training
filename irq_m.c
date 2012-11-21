/* Playing with:
 * - Interrupts
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#define IRQ_N	12	// running in virtualbox, 
					// so using PS/2 irq for this time
					
struct irqm_data {
	int magic_var;
	struct tasklet_struct tasklet;
} ctx;

// TODO:
// first trying handle with tasklet
// then trying threaded_irq

static void on_irq_tasklet(unsigned long arg)
{
	struct irqm_data *data = (struct irqm_data *)arg;
	unsigned long j = jiffies;

	++data->magic_var;
	printk("(^_^) ***IRQ*** jiffies: %li, magic_var: %3d%s\n",
			j, data->magic_var, in_interrupt() ? ", in_irq" : "");
}

static irqreturn_t handler(int irq, void *dev_id)
{
	struct irqm_data *data = dev_id;
	
	tasklet_schedule(&data->tasklet);

	return IRQ_HANDLED;
}

static int __init irqm_init(void)
{
	struct irqm_data *data = &ctx;

	tasklet_init(&data->tasklet, on_irq_tasklet, (unsigned long)data);
	// TODO:
	// Error handling
	request_irq(IRQ_N, handler, IRQF_SHARED, "mega_irq_handler", data);

	return 0;
}

static void __exit irqm_exit(void)
{
	struct irqm_data *data = &ctx;

	free_irq(IRQ_N, data);
	tasklet_kill(&data->tasklet);
}

module_init(irqm_init);
module_exit(irqm_exit);

MODULE_LICENSE("Dual BSD/GPL");
