#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/types.h>

#define MAX_DEV 1
/*
 *	switch(cmd)
 *	{
 *		case i
 *		case ii.....
 *	}
 *	#define IO_BUFFER 0
 *	#define IO_READ 1
 *
 *
 *
 */
#define CASE_ZERO 0
#define CASE_ONE 1
#define CASE_TWO 2
#define CASE_THREE 3

static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("MYCHARDEV: Device open\n");
    return 0;
}

static int	dev_release(struct inode *inode, struct file *file)
{
    pr_info("MYCHARDEV: Device close\n");
    return 0;
}


static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	char buffer[8];
	char copy_to_usr[16] = "Roger That";
	/*get data */
	pr_info("FUNCTION: %s & cmd val is %d\n",__func__,cmd);
	switch(cmd) {
        case CASE_ZERO:
			pr_info("excuted case0 sucessed\n");
			break;
        case CASE_ONE:
			pr_info("excuted cased1 sucessed\n");
			break;
        case CASE_TWO:
			pr_info("excuted cased2 sucessed\n");
			break;
		default:
            pr_info("Unvailbe Command,function don't support \n");
			break;
    }
   	pr_info("MYCHARDEV12323213: Device ioctl\n");
    return 0;
}

static const struct file_operations mychardev_fops = {
    .owner      = THIS_MODULE,
    .open       = dev_open,
    .release    = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

struct mychar_device_data {
    struct cdev cdev;
};

static int dev_major = 0;
static struct class *mychardev_class = NULL;
static struct mychar_device_data mychardev_data;


static int __init mychardev_init(void)
{
	int err;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, 1, "mychardev");
	if(err != 0){
		pr_err("allocation register failed\n");
	}

    dev_major = MAJOR(dev);
	/*	*/
    mychardev_class = class_create(THIS_MODULE, "mychardev");
	/* Intializtion*/
    cdev_init(&mychardev_data.cdev, &mychardev_fops);
    mychardev_data.cdev.owner = THIS_MODULE;
    cdev_add(&mychardev_data.cdev, MKDEV(dev_major, 0), 1);
    device_create(mychardev_class, NULL, MKDEV(dev_major, 0), NULL, "mychardev");
	pr_info("executed function: %s\n",__func__);
    return 0;
}

static void __exit mychardev_exit(void)
{
    device_destroy(mychardev_class, MKDEV(dev_major, 0));
    class_unregister(mychardev_class);
    class_destroy(mychardev_class);
    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
	pr_info("executed function: %s\n",__func__);
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul Chen <eric2002123200@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
