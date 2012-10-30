#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

/* Parameters */
static char *paramS = "";
static int param1 = 0;
static int param2 = 1;

module_param(paramS, charp, S_IRUGO);
module_param(param1, int, S_IRUGO);
module_param(param2, int, S_IRUGO);

static int hello_init(void)
{
    // Trying to divide param1 by param2
    // and print result to console
    if ( param2 != 0 ) {
        printk(KERN_ALERT "Hi!\n" "%d / %d = %d\n", param1, param2, param1/param2);
    }
    else {
        printk(KERN_ALERT "Hi!\n" "Incorrect parameter! Divizion by zero!\n");        
    }
    
    // Printing paramS to console
    printk(KERN_ALERT "Parameter paramS is \"%s\"", paramS);

    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Bye!\n");
}

module_init(hello_init);
module_exit(hello_exit);
