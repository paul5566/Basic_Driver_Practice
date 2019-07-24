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
#define GET_USR 0
#define PUT_USR 1
#define COPY_FROM_USR 2
#define COPY_TO_USR 3

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

static ssize_t dev_read(struct file *fil, char __user * buf, size_t size,
loff_t * ppos)
{
	pr_info("MYCHARDEV: Device READ\n");
	return 0;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	//int return_cal = 1;
	//int buffer_val = 0;
	int err;
	char buffer[8];
	char copy_to_usr[16] = "Roger That";
	/*get data */
	pr_info("the cmd val is %d\n",cmd);
	switch(cmd) {
        case 0:
			if (err != 0){
				pr_err("%s: get user failed\n",__func__);
				break;
			}
			pr_info("excuted GET USER sucessed\n");
			break;
        case 1:
			if (err != 0){
				pr_err("%s: put user failed\n",__func__);
				break;
			}
			pr_info("excuted PUT USER sucessed\n");
            break;
        case 2:
 			err = copy_from_user(&buffer ,(void __user *)arg, sizeof(buffer));
			if (err != 0){
				pr_err("%s: copy from user failed\n",__func__);
				break;
			}
			pr_info("buffer received from user: %s\n", &buffer);
            break;
        case 3:
			err = copy_to_user((void __user *)arg,&copy_to_usr,sizeof(copy_to_usr));
			if (err != 0){
				pr_err("%s: copy to user failed\n",__func__);
				break;
			}
        default:
            pr_info("Unvailbe Command,function don't support \n");
    }
   	pr_info("MYCHARDEV: Device ioctl\n");
    return 0;
}

static const struct file_operations mychardev_fops = {
    .owner      = THIS_MODULE,
    .open       = dev_open,
    .release    = dev_release,
    .unlocked_ioctl = dev_ioctl,
	.read = dev_read
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
