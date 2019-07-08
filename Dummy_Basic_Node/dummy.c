/* riowd.c - driver for hw watchdog inside Super I/O of RIO
 *
 * Copyright (C) 2001, 2008 David S. Miller (davem@davemloft.net)
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

/* RIO uses the NatSemi Super I/O power management logical device
 * as its' watchdog.
 *
 * When the watchdog triggers, it asserts a line to the BBC (Boot Bus
 * Controller) of the machine.  The BBC can only be configured to
 * trigger a power-on reset when the signal is asserted.  The BBC
 * can be configured to ignore the signal entirely as well.
 *
 * The only Super I/O device register we care about is at index
 * 0x05 (WDTO_INDEX) which is the watchdog time-out in minutes (1-255).
 * If set to zero, this disables the watchdog.  When set, the system
 * must periodically (before watchdog expires) clear (set to zero) and
 * re-set the watchdog else it will trigger.
 *
 * There are two other indexed watchdog registers inside this Super I/O
 * logical device, but they are unused.  The first, at index 0x06 is
 * the watchdog control and can be used to make the watchdog timer re-set
 * when the PS/2 mouse or serial lines show activity.  The second, at
 * index 0x07 is merely a sampling of the line from the watchdog to the
 * BBC.
 *
 * The watchdog device generates no interrupts.
 */


MODULE_AUTHOR("David S. Miller <davem@davemloft.net>");
MODULE_DESCRIPTION("Hardware watchdog driver for Sun RIO");
MODULE_SUPPORTED_DEVICE("watchdog");
MODULE_LICENSE("GPL");

#define DRIVER_NAME	"dummy_testing"

struct dummy {
	void __iomem		*regs;
	spinlock_t		lock;
};

static struct dummy *dummy_device;

static int dummy_open(struct inode *inode, struct file *filp)
{
	nonseekable_open(inode, filp);
	pr_info("Dummy Open\n");
	return 0;
}

static int dummy_release(struct inode *inode, struct file *filp)
{
	pr_info("Dummy Release->END\n");
	return 0;
}
static const struct file_operations dummy_fops = {
	.owner =		THIS_MODULE,
	.open =			dummy_open,
	.release =		dummy_release,
};

static struct miscdevice dummy_miscdev = {
	.minor	= WATCHDOG_MINOR,
	.name	= "watchdog",
	.fops	= &dummy_fops
};

static int dummy_probe(struct platform_device *op)
{


	struct dummy *p;
	int err = -EINVAL;
	
	err = -ENOMEM;
	p = devm_kzalloc(&op->dev, sizeof(*p), GFP_KERNEL);
	if (!p)
		goto out;


	if (!p->regs) {
		pr_err("Cannot map registers\n");
		goto out;
	}

	/* Make miscdev useable right away */
	dummy_device = p;
/*
 *
 *	This is not correctly!!!! Think about the type of platform device
 *	driver is different. 
 *
 *
 * */
	err = misc_register(&dummy_miscdev);
	if (err) {
		pr_err("Cannot register watchdog misc device\n");
		goto out_iounmap;
	}


	platform_set_drvdata(op, p);
	return 0;

out_iounmap:
	dummy_device = NULL;

out:
	return err;
}

static int dummy_remove(struct platform_device *op)
{
	struct dummy *p = platform_get_drvdata(op);

	misc_deregister(&dummy_miscdev);

	return 0;
}

static struct platform_driver dummy_driver = {
	.driver = {
		.name = DRIVER_NAME,
	},
	.probe		= dummy_probe,
	.remove		= dummy_remove,
};

module_platform_driver(dummy_driver);
