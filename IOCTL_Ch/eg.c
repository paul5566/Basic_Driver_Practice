#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/types.h>

#define MAX_DEV 1

static int dev_open(struct inode *inode, struct file *file);
static int dev_release(struct inode *inode, struct file *file);
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

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
	/*get data */
    copy_from_user(&buffer ,(uint32_t*) arg, sizeof(buffer));
   	pr_info("buffer received from user%s\n", &buffer);
    pr_info("MYCHARDEV: Device ioctl\n");
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
static struct mychar_device_data mychardev_data[0];


static int __init mychardev_init(void)
{
	int err;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "mychardev");

    dev_major = MAJOR(dev);

    mychardev_class = class_create(THIS_MODULE, "mychardev");
	/* Intializtion*/
    cdev_init(&mychardev_data[0].cdev, &mychardev_fops);
    mychardev_data[0].cdev.owner = THIS_MODULE;
    cdev_add(&mychardev_data[0].cdev, MKDEV(dev_major, 0), 1);
    device_create(mychardev_class, NULL, MKDEV(dev_major, 0), NULL, "mychardev");
	pr_info("executed function:%s\n",__func__);
    return 0;
}

static void __exit mychardev_exit(void)
{
    device_destroy(mychardev_class, MKDEV(dev_major, 0));
    class_unregister(mychardev_class);
    class_destroy(mychardev_class);
    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
	pr_info("executed function:%s\n",__func__);
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Kutkov <elenbert@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
