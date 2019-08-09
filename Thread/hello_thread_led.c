#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>//wake_up_process()
#include <linux/kthread.h>//kthread_create()¡¢kthread_run()
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/mutex.h>
#include <linux/delay.h>


# define led_green 137


static int blink_led(void)
{
	int t;

	//if the led_green is not valid
	if ( !gpio_is_valid(led_green) ){
		gpio_free(led_green);
		return -1;
	}
	gpio_direction_output(led_green,true);
	//for loop to blink led
	for(t = 0;t < 10 ;t++){
		msleep(100);
		gpio_set_value(led_green, 1);
		msleep(100);
		gpio_set_value(led_green, 0);
		msleep(100);
	}
	return 0;
}


static int foo_thread(void *d)
{
	int i;
	int led_failed;

	// TODO: Put LED operation here

	for (i = 0; i < 5; ++i) {
		pr_info("Hi! The %s (%d)nd\n", __func__, i);
		led_failed = blink_led();
		if(led_failed){
			pr_info("Function:blink led failed\n");
			return led_failed;
		}
	}

	return 0;
}

static __init int my_start_thread(void)
{
	struct task_struct *t;

	t = kthread_run(foo_thread, NULL, "foo_thread");
	if (IS_ERR(t)) {
		pr_err("foo thread create failed!\n");
		return -1;
	}
	return 0;
}

static __exit void my_end_thread(void)
{
	gpio_set_value(led_green,0);
	pr_info("end of the module\n");
}
module_init(my_start_thread);
module_exit(my_end_thread);

MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
