//==============================================
//This is driver, using the dummy node device driver to implement it
////////////////////////

static long foo_ioctl(struct file *f, unsigned int cmd, unsigned long args)
{
	char buf[128];

	switch (cmd) {
	case CMD_MY_READ:
		copy_from_user(bug, (_user)args, 8);
		pr_info("%s\n", buf);
		break;
	case CMD_MY_WRITE:
		// do write jobs
		break;
	...
	...

	return 0;
}

struct file_operations foo_ops = {
	.open = foo_open,
	.release = foo_close,
	.compat_ioctl = foo_ioctl,
...
...
};

