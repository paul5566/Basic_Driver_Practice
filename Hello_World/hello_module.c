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




#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/mutex.h>
#include <linux/delay.h>






static int __init hello_init(void)
{
	pr_info("Hello World module Intial\n");
	return 0;
}


static void __exit hello_exit(void)
{
	pr_info("end of the module\n");
}
/*
 *  load a Linux driver
 */
module_init(hello_init);
/*
 * unload the Linux driver
 */
module_exit(hello_exit);


MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");

