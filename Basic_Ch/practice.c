#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define DRIVER_NAME "practice_ch"
static int chr_devs = 1;  /* device count */
static int chrdev_major = 0; /* dynamic allocation */
/*what's the mpoudle initialization*/
//module_param(chrdev_major, uint, 0);

/*
	待會在改成struct struct

	struct mychar_device_data {
    	struct cdev cdev;
	};
*/
//chrdev_cdev is a structure type of cdev
static struct cdev chrdev_cdev = {
	.owner = THIS_MODULE,
};

static int device_open(struct inode *inode, struct file *file)
{
    pr_info("MYCHARDEV: Device open\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    pr_info("MYCHARDEV: Device close\n");
    return 0;
}

/*
 * Intialized a structure which form is same
 * as file_operation
 */
struct file_operations fops = {
       .open = device_open,
       .release = device_release
};


static int __init mychardev_init(void)
{
	/*A kind of number regiater define as type dev_t*/

	dev_t chrdev = MKDEV(chrdev_major, 0);
    alloc_chrdev_region(&chrdev, 0, chr_devs, DRIVER_NAME);
    chrdev_major = MAJOR(chrdev);
    cdev_init(&chrdev_cdev, &fops);
    cdev_add(&chrdev_cdev, MKDEV(chrdev_major, 0), chr_devs);

    return 0;
}



static void __exit mychardev_exit(void)
{
	dev_t chrdev = MKDEV(chrdev_major, 0);

	cdev_del(&chrdev_cdev);
    unregister_chrdev_region(chrdev, chr_devs);
	pr_info("%s driver is removed.\n",DRIVER_NAME);
}




MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul <eric20021232000@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
