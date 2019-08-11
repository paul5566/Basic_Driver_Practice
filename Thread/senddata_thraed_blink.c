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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>//wake_up_process()
#include <linux/kthread.h>//kthread_create()﹜kthread_run()
#include <linux/version.h>
#include <linux/delay.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/mutex.h>
#include <linux/delay.h>


# define led_green 137

static int led_thread(void *d)
{
	int i;
	int *times = (int*)d;
	//if the led_green is not valid
	if ( !gpio_is_valid(led_green) ){
		gpio_free(led_green);
		return -1;
	}
	gpio_direction_output(led_green,true);
	pr_info("aaaaaaaaThe times is %d\n",*times);
	// TODO: Put LED operation here

	//for loop to blink led
	for(i = 0;i < *times; i++){
		gpio_set_value(led_green, 1);
		pr_info("pin on:%dnd",i);
		msleep(100);
		gpio_set_value(led_green, 0);
		pr_info("pin off:%dnd",i);
		msleep(100);
	}
	return 0;
}



static int __init gpio_hello_init(void)
{
	struct task_struct *t;
	int led_loop = 100;

	t = kthread_run(led_thread, &led_loop, "led_thread");
	if (IS_ERR(t)) {
		pr_err("foo thread create failed!\n");
		return -1;
	}
	//for loop to blink led
	return 0;
}


static void __exit gpio_hello_exit(void)
{
	gpio_set_value(led_green,0);
	pr_info("end of the module\n");
	gpio_free(led_green);
}
//load module
module_init(gpio_hello_init);
//exit module
module_exit(gpio_hello_exit);



MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");

