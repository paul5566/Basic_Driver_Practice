#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>



MODULE_LICENSE("Dual BSD/GPL");

static int __init gpio_hello_init(void)
{
	pr_info("start of the module\n");
	return 0;
}


static void __exit gpio_hello_exit(void)
{
	pr_info("end of the module\n");
}
/*
 *  load a Linux driver
 */
module_init(gpio_hello_init);
/*
 * unload the Linux driver
 */
module_exit(gpio_hello_exit);
MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("GPL");
