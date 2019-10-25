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

/*
 *	When we create the thread, the most horriable thing is
 *	the race condition,hence it's very stupid to declare the
 *	variable that other resource may use it
 */


int __init timer_init(void)
{
	static struct timeval start={
		.tv_sec=0,
		.tv_usec=0
	};

	static struct timeval end={
		.tv_sec=0,
		.tv_usec=0
	};

	//printk("<0>do_gettimeofday test begin.\n");
	do_gettimeofday(&start);
	pr_info("the start time in %ldsecs\n",start.tv_sec);
	pr_info("the start time in %ldusec\n",start.tv_usec);
	msleep(3000);
	do_gettimeofday(&end);
	pr_info("%ld ",end.tv_usec-start.tv_usec);
	pr_info("the end time in %ldsecs\n",end.tv_sec);
	pr_info("the end time in %ldusec\n",end.tv_usec);
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

