/*======================================================================
    A globalmem driver as an example of char device drivers

    The initial developer of the original code is Baohua Song
    <author@linuxdriver.cn>. All Rights Reserved.
======================================================================*/



#include <linux/version.h>

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>

//  error: implicit declaration of function `kfree`
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 19, 0)
#include <asm/system.h>
#else
#include <linux/slab.h>
#endif

#include <asm/uaccess.h>

#define GLOBALMEM_SIZE      0x1000	/* GLOBALMEM_SIZE*/
#define MEM_CLEAR           0x1     /* MEME CLEAR*/
#define GLOBALMEM_MAJOR     300    /* the MAJOR NUMBER what we assign to GLOBALMEM*/


/*device number of  globalmem*/
static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int, S_IRUGO);


/*  the structure of globalmem__dev' od character device driver*/
struct globalmem_dev
{

    struct cdev cdev;                         /*  the globalmem inherit the strucr cdev  */
    unsigned char mem[GLOBALMEM_SIZE];        /*      */

};

struct globalmem_dev *globalmem_devp;	/* globalmem_devp is the ptr point to struct globalmem*/


/*  open: define in file_operation    */
int globalmem_open(struct inode *inode, struct file *filp)
{
    filp->private_data = globalmem_devp;

    return 0;
}

/*  release: define in file_operaton  */
int globalmem_release(struct inode *inode, struct file *filp)
{
    return 0;
}
/*
 *
 * Before the Linux Version 2.6.36, the structure of
 * file_operation have the memeber ioctl, but in the new verison
 * the ioctl member is defined in
 * (a)compat_ioctl
 * (b)unlock_ioctl
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
/* ioctl function*/
static int globalmem_ioctl(
        struct inode *inodep,
        struct file *filp,
        unsigned int cmd,
        unsigned long arg)
{
#else
//long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
//long (*compat_ioctl) (struct file *file, unsigned int cmd, unsigned long arg)
static long globalmem_compat_ioctl(
        struct file *filp,
        unsigned int cmd,
        unsigned long arg)
{
    //struct inode *inode = file->f_dentry->d_inode;
    struct inode *inode = inode = file_inode(filp);
#endif

    struct globalmem_dev *dev = filp->private_data;

    switch (cmd)
    {
        case MEM_CLEAR://MEM_CLEAR is assigned to one
        {
            memset(dev->mem, 0, GLOBALMEM_SIZE);
            printk(KERN_INFO "globalmem is set to zero\n");
            break;
        }

        default:
        {
            return  - EINVAL;
        }
    }

    return 0;
}

/*Read Function*/
static ssize_t globalmem_read(
        struct file *filp,
        char __user *buf,
        size_t      size,
        loff_t      *ppos)
{
    unsigned long           p =  *ppos;                /*   ????once = 0, twice = GLOBALMEM_SIZE  */
    unsigned int            count = size;              /*   GLOBALMEM_SIZE */
    int                     ret = 0;
    struct globalmem_dev    *dev = filp->private_data; /*get the ptr which point to structure ""*/

    /* analyze and scatch the efficient length of code */
    if (p >= GLOBALMEM_SIZE)
    {
        printk("*ppos = %d\n", p);
        /*
 		 *	 ~reference~
         * https://zhidao.baidu.com/question/136829890072623445.html
         * http://bbs.chinaunix.net/thread-1961861-1-1.html
         * http://blog.csdn.net/qiaoliang328/article/details/4874238
         * Fix bug when `cat /dev/ XXX`
         *
         * return count ? 0 : -ENXIO;  if the condition is satisfied than return -ENXIO, 
         * which means No such device or address
         */
        return count ? -ENXIO : 0;
    }

    if (count > GLOBALMEM_SIZE - p)
    {
        count = GLOBALMEM_SIZE - p;
    }

    /*  First of all, the cat command would read the data in device driver
 	 *  	when the return value is >0,
     *      `cat` would keeo doing the 	`read operation` in the open device
     *      open every time read (4096 chr)
     */
/*
	"User Space"-> "Kernel Spce"
    Using dev->mem of offset `p` as the beginninf of the memory,
	copy to count to the user spcae buffer
*/

	if (copy_to_user(buf, (void*)(dev->mem + p), count))
    {
        ret =  - EFAULT;
    }
    else
    {
        *ppos += count;
        ret = count;

        printk(KERN_INFO "read %d bytes(s) from %d\n", count, p);
    }
    return ret;
}

/*write function*/
static ssize_t globalmem_write(
        struct file         *filp,
        const char __user   *buf,
        size_t              size,
        loff_t              *ppos)
{
    unsigned long p =  *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalmem_dev *dev = filp->private_data; /*get the ptr which point to glambalmem structure*/

    /*  analyze the scratch the length ?? */
    if (p >= GLOBALMEM_SIZE)
    {
        return count ?  - ENXIO: 0;
    }

    if (count > GLOBALMEM_SIZE - p)
    {
        count = GLOBALMEM_SIZE - p;
    }
    if (copy_from_user(dev->mem[p], buf, count) != 0)
    {
        ret =  - EFAULT;
    }
    else
    {
        *ppos += count;
        ret = count;

        printk(KERN_INFO "written %d bytes(s) from %d\n", count, p);
    }
    return ret;
}

/* seek funcion*/
static loff_t globalmem_llseek(
        struct file *filp,
        loff_t      offset,
        int         orig)
{
    loff_t ret = 0;
    switch (orig)
    {
        case 0:   /*the offset starts???*/
            if (offset < 0)
            {
                ret =  - EINVAL;
                break;
            }

            if ((unsigned int)offset > GLOBALMEM_SIZE)
            {
                ret =  - EINVAL;
                break;
            }

            filp->f_pos = (unsigned int)offset;

            ret = filp->f_pos;

            break;

        case 1:   /*the document's offset*/
            if ((filp->f_pos + offset) > GLOBALMEM_SIZE)
            {
                ret =  - EINVAL;
                break;
            }
            if ((filp->f_pos + offset) < 0)
            {
                ret =  - EINVAL;
                break;
            }

            filp->f_pos += offset;
            ret = filp->f_pos;

            break;

        default:

            ret =  - EINVAL;
            break;
    }

    return ret;
}

/*structure of operation*/
static const struct file_operations globalmem_fops =
{
    .owner = THIS_MODULE,
    .llseek = globalmem_llseek,
    .read = globalmem_read,
    .write = globalmem_write,

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
    .ioctl = globalmem_ioctl,
#else
    //.unlocked_ioctl = globalmem_ioctl,
    .compat_ioctl = globalmem_compat_ioctl,
#endif

    .open = globalmem_open,
    .release = globalmem_release,
};

/*Intialize and register cdev*/
static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
    int err, devno = MKDEV(globalmem_major, index);

    cdev_init(&dev->cdev, &globalmem_fops);

    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &globalmem_fops;

    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding LED%d", err, index);
    }
}

/*	module intializtion*/
int globalmem_init(void)
{
    int result;
    dev_t devno = MKDEV(globalmem_major, 0);

    /* apply MAJOR number static */
    if (globalmem_major != 0)
    {
        result = register_chrdev_region(devno, 1, "globalmem");
        printk(KERN_INFO "register char device drivers [globalmem], MAJOR = %d\n", globalmem_major);
    }
    else  /* dynamic apply MAJOR number*/
    {
        result = alloc_chrdev_region(&devno, 0, 1, "globalmem");
        globalmem_major = MAJOR(devno);
        printk(KERN_INFO "alloc char device drivers [globalmem], MAJOR = %d\n", globalmem_major);
    }

    if (result < 0)
    {
        return result;
    }

    /* malloc memory to apply globalmem device driver*/
    globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
    if (!globalmem_devp)    /* register failed*/
    {
        result =  - ENOMEM;
        unregister_chrdev_region(devno, 1);

        return result;
    }
    memset(globalmem_devp, 0, sizeof(struct globalmem_dev));

    globalmem_setup_cdev(globalmem_devp, 0);

    return 0;
}

/*Module exit*/
void globalmem_exit(void)
{
    cdev_del(&globalmem_devp->cdev);   /* delete the cdev*/

    kfree(globalmem_devp);     /*free the dynamic globalmeme_devp*/

    unregister_chrdev_region(MKDEV(globalmem_major, 0), 1); /*release(unregister) the MAJOR number*/
}


//Driver Information
#define DRIVER_VERSION  "1.0.0"
#define DRIVER_AUTHOR   "Gatieme @ AderStep Inc..."
#define DRIVER_DESC     "Linux input module for Elo MultiTouch(MT) devices"
#define DRIVER_LICENSE  "Dual BSD/GPL"

// Kernel Module Information
MODULE_VERSION(DRIVER_VERSION);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);


module_init(globalmem_init);
module_exit(globalmem_exit);
