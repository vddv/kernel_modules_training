#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static void null_pointer(void)
{
    int *boo = NULL;

    *boo = 0;
}

static void division_by_zero(void)
{
    int shi = 0;

    shi /= shi;
}

static int boo_init(void)
{
    division_by_zero();
    // null_pointer();

    return 0;
}

static void boo_exit(void)
{
}

module_init(boo_init);
module_exit(boo_exit);
