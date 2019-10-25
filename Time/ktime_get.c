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
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/clkdev.h>



/*
	 while (!(readl_relaxed(pll->con_reg) & PLL46XX_LOCKED)) {
		ktime_t delta = ktime_sub(ktime_get(), start);

		if (ktime_to_ms(delta) > PLL_TIMEOUT_MS) {
			pr_err("%s: could not lock PLL %s\n",
					__func__, clk_hw_get_name(hw));
			return -EFAULT;
		}

		cpu_relax();
	}
 */

static int __init hello_init(void)
{

	ktime_t start;

	start = ktime_get();
	pr_info("the start time is %ld\n", ktime_to_ns(start));
	msleep(10000);
	/*
	do{

	}while();
	*/
	ktime_t delta = ktime_sub(ktime_get(),start);
	pr_info("the diff time is %ld\n", ktime_to_ns(delta));
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




