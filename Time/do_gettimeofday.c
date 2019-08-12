#include <linux/module.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <linux/errno.h>
#include <linux/delay.h>


static struct timeval start={
	.tv_sec=0,
	.tv_usec=0
};
static struct timeval end={
	.tv_sec=0,
	.tv_usec=0
};



int __init timer_init(void)
{
	printk("<0>do_gettimeofday test begin.\n");
	do_gettimeofday(&start);
	msleep(3000);
	do_gettimeofday(&end);
	pr_info("The time passed in %ld second \n",end.tv_usec-start.tv_usec);//sec
	return 0;
}


void __exit timer_exit(void)
{
	pr_info("timer exit\n");
}

module_init(timer_init);

module_exit(timer_exit);




MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");

