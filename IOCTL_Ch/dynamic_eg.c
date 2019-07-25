#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/types.h>


#define MAX_DEV 1
#define CASE_ZERO 0
#define CASE_ONE 1


static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("DYNCHDEV: Device open\n");
    return 0;
}

static int	dev_release(struct inode *inode, struct file *file)
{
    pr_info("DYNCHDEV: Device close\n");
    return 0;
}


static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	char buffer[8];
	/*get data */
	pr_info("FUNCTION: %s & cmd's val is %d\n",__func__,cmd);
	switch(cmd) {
        case CASE_ZERO:
			pr_info("excuted case0 sucessed\n");
			break;
        case CASE_ONE:
			pr_info("excuted cased1 sucessed\n");
			break;
        default:
            pr_info("Unvailbe Command,function don't support \n");
			break;
    }
   	pr_info("DYNCHDEV: Device ioctl\n");
    return 0;
}

static const struct file_operations dynamic_fops = {
    .owner      = THIS_MODULE,
    .open       = dev_open,
    .release    = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

static struct cdev dynamic_chdev = {
	.owner = THIS_MODULE
};

static int dev_major = 0;
static struct class *dynamic_class = NULL;
/*module entry point*/
static int __init mychardev_init(void)
{
	int err;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, 1, "dyn_ch_dev");
	if(err != 0){
		pr_err("allocation register failed\n");
	}

    dev_major = MAJOR(dev);
	/*class create*/
    dynamic_class = class_create(THIS_MODULE,"dyn_ch_dev");
	/* Intializtion*/
    cdev_init(&dynamic_chdev, &dynamic_fops);
	cdev_add(&dynamic_chdev, MKDEV(dev_major, 0), 1);
    device_create(dynamic_class, NULL, MKDEV(dev_major, 0), NULL,"dyn_ch_dev");
	pr_info("executed function: %s\n",__func__);
    return 0;
}
/* module exit point*/
static void __exit mychardev_exit(void)
{
    device_destroy(dynamic_class, MKDEV(dev_major, 0));
    class_unregister(dynamic_class);
    class_destroy(dynamic_class);
    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
	pr_info("executed function: %s\n",__func__);
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul Chen <eric2002123200@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
