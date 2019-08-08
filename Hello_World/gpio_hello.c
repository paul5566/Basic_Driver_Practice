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


static DEFINE_MUTEX(gpio_lock);


/*
struct gpio_edm_data {
		unsigned int gpio;
		unsigned int value;
		const char * direction;
		const char *desc;
};
*/
/*
 truct platform_device {
	const char	*name;
	int		id;
	bool		id_auto;
	struct device	dev;
	u32		num_resources;
	struct resource	*resource;

	const struct platform_device_id	*id_entry;
	char *driver_override; // Driver name to force a match

	// MFD cell pointer
	struct mfd_cell *mfd_cell;

	// arch specific additions
	struct pdev_archdata	archdata;
};
 */



MODULE_LICENSE("Dual BSD/GPL");

static int __init gpio_hello_init(void)
{
	int error;
	int t;

	if ( gpio_is_valid(137) ){
			pr_info("start of the module\n");
			error = gpio_request(137,"gpio-led");
			if (error < 0) {
				pr_info("Failed to request GPIO %d, error %d\n",137, error);
				return error;
			}
			gpio_direction_output(137,true);

			for(t = 0;t < 5 ;t++){
				gpio_set_value(137, true);
				pr_info("wait\n");
				msleep(50);
				gpio_set_value(137, false);
			}
			return 0;
	}
	//failed issue: The gpio_hello_world is not valid
	else	{
		gpio_free(138);
		return error;
	}
}


static void __exit gpio_hello_exit(void)
{
	gpio_set_value(137,false);
	pr_info("end of the module\n");
	gpio_free(137);
}
//load module
module_init(gpio_hello_init);
//exit module
module_exit(gpio_hello_exit);



MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("GPL");
