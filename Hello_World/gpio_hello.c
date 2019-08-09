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


# define led_green 137

static int __init gpio_hello_init(void)
{
	int error;
	int t;

	pr_info("start of the module\n");
	error = gpio_request(led_green,"gpio-led");
	if (error < 0) {
				pr_info("Failed to request GPIO %d, error %d\n",137, error);
				return error;
	}
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

