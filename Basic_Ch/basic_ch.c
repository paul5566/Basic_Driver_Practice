#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "chrdev_test"

static int chr_devs = 2;  /* device count */
static int chrdev_major = 0; /* dynamic allocation */
module_param(chrdev_major, uint, 0);
static struct cdev chrdev_cdev;

static int chrdev_open(struct inode *inode, struct file *file)
{
    printk("%s: major %d minor %d (pid %d)\n",
               __func__,
               imajor(inode),
               iminor(inode),
               current->pid);

    inode->i_private = inode;
    file->private_data = file;

    printk("i_private=%p private_data=%p\n",
            inode->i_private, file->private_data);

    return 0;
}

static int chrdev_close(struct inode *inode, struct file *file)
{
    printk("%s: major %d minor %d (pid %d)\n",
            __func__,
            imajor(inode),
            iminor(inode),
            current->pid);

    printk("i_private=%p private_data=%p\n",
            inode->i_private, file->private_data);
    return 0;
}

struct file_operations chrdev_fops = {
    .open = chrdev_open,
    .release = chrdev_close,
};

static int chrdev_init(void)
{
    dev_t chrdev = MKDEV(chrdev_major, 0);
    int major;

    alloc_chrdev_region(&chrdev, 0, chr_devs, DRIVER_NAME);
    chrdev_major = major = MAJOR(chrdev);
    cdev_init(&chrdev_cdev, &chrdev_fops);
    chrdev_cdev.owner = THIS_MODULE;
    cdev_add(&chrdev_cdev, MKDEV(chrdev_major, 0), chr_devs);

    printk(KERN_ALERT "%s driver(major %d) installed.\n", DRIVER_NAME, major);

    return 0;
}

static void chrdev_exit(void)
{
    dev_t chrdev = MKDEV(chrdev_major, 0);

    cdev_del(&chrdev_cdev);
    unregister_chrdev_region(chrdev, chr_devs);

    printk(KERN_ALERT "%s driver removed.\n", DRIVER_NAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);

