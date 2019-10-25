#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFSIZE  100


static struct proc_dir_entry *ent;

static ssize_t mywrite(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos)
{
	printk( KERN_DEBUG "write handler\n");
	return -1;
}
static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos)
{
	printk( KERN_DEBUG "read handler\n");
	return 0;
}
static struct file_operations myproc_fops =
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};
static int proc_init(void)
{
	ent=proc_create("myproc",0660,NULL,&myproc_fops);
	return 0;
}
static void proc_exit(void)
{
	proc_remove(ent);
}


module_init(proc_init);
module_exit(proc_exit);

MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");

