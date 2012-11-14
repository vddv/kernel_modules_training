#include <linux/init.h>
#include <linux/module.h>

#include "boomodule.h"

MODULE_LICENSE("Dual BSD/GPL");

static void null_pointer(void)
{
    *(int *)NULL = 0;
}

static void division_by_zero(void)
{
    int shi = 0;

    shi /= shi;
}

static int boo_init(void)
{
    DLOG("Division by zero!");
    division_by_zero();    

    DLOG("Null-pointer dereference!");
    null_pointer();
    	
    return 0;
}

static void boo_exit(void)
{
}

module_init(boo_init);
module_exit(boo_exit);
