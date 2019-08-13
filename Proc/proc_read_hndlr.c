#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFSIZE  128
static int irq=20;
module_param(irq,int,0660);

static int mode=1;
module_param(mode,int,0660);

static struct proc_dir_entry *ent;



/*
 * 64bit machine  size_t is "unsigned long"
 * 32bit machine size_t is "unsigned int"
 */
static ssize_t hndlr_write(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos)
{
	int num,c,i,m;
	char buf[BUFSIZE];
	if(*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	if(copy_from_user(buf, ubuf, count))
		return -EFAULT;
	num = sscanf(buf,"%d %d",&i,&m);
	if(num != 2)
		return -EFAULT;
	irq = i;
	mode = m;
	c = strlen(buf);
	*ppos = c;
	return c;
}

static ssize_t hndlr_read(struct file *file, char __user *ubuf,size_t count, loff_t *ppos)
{
	char buf[BUFSIZE];
	unsigned long len=0;
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	len += sprintf(buf,"irq = %d\n",irq);
	len += sprintf(buf + len,"mode = %d\n",mode);

	if(copy_to_user(ubuf,buf,len))
		return -EFAULT;
	*ppos = len;
	return len;
}

static struct file_operations myops =
{
	.owner = THIS_MODULE,
	.read = hndlr_read,
	.write = hndlr_write,
};

static int hndlr_init(void)
{
	ent=proc_create("read_hndlr",0660,NULL,&myops);
	pr_info( "hello...\n");
	return 0;
}

static void hndlr_cleanup(void)
{
	proc_remove(ent);
	pr_info(KERN_WARNING "bye ...\n");
}

module_init(hndlr_init);
module_exit(hndlr_cleanup);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Boring Chen");
