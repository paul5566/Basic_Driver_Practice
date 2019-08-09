#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>//wake_up_process()
#include <linux/kthread.h>//kthread_create()¡¢kthread_run()
#include <linux/version.h>
#include <linux/delay.h>


//#include <err.h>

static int foo_thread(void *d)
{
	int i;

	// TODO: Put LED operation here

	for (i = 0; i < 10; ++i) {
		pr_info("Hi! I'm %s (%d)\n", __func__, i);
		msleep(1000);
	}

	return 0;
}

static __init int my_start_thread(void)
{
	struct task_struct *t;

	t = kthread_run(foo_thread, NULL, "foo_thread");
	if (IS_ERR(t)) {
		pr_err("foo thread create failed!\n");
		return -1;
	}
	return 0;
}

static __exit void my_end_thread(void)
{
	pr_info("end of th thread\n");
}
module_init(my_start_thread);
module_exit(my_end_thread);

MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
