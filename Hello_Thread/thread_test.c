#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>//wake_up_process()
#include <linux/kthread.h>//kthread_create()¡¢kthread_run()
#include <linux/version.h>
//#include <err.h>

static struct task_struct * _task1;
static struct task_struct * _task2;
static struct task_struct * _task3;

static int thread_func(void *data)
{
        int j,k;
        int timeout = HZ;
        wait_queue_head_t timeout_wq;
		char * data1=(char*)data;
        static int i = 0;
		i++;
        j = 0;
        k = i;
        printk("thread_func %d started\n", i);
   		printk("data=%s\n", data1);
        init_waitqueue_head(&timeout_wq);
        while(!kthread_should_stop())
        {
			DEFINE_WAIT(wait);
			prepare_to_wait(&timeout_wq, &wait, TASK_INTERRUPTIBLE);
			timeout = schedule_timeout(timeout);
			finish_wait(&timeout_wq, &wait);
        	printk("[%d]sleeping..%d\n", k, j++);
        }
        return 0;
}

static __init int my_start_thread(void)
{

        //_task = kthread_create(thread_func, NULL, "thread_func2");
        //wake_up_process(_task);
        _task1 = kthread_run(thread_func, NULL, "thread_func1");
        _task2 = kthread_run(thread_func, NULL, "thread_func2");
        _task3 = kthread_run(thread_func, NULL, "thread_func3");
        if (!IS_ERR(_task1))
        {
                printk("kthread_create done\n");
        }
        else
        {
                printk("kthread_create error\n");
        }

        return 0;
}

static __exit void my_end_thread(void)
{
        int ret = 0;
        ret = kthread_stop(_task1);
        printk("end thread. ret = %d\n" , ret);
        ret = kthread_stop(_task2);
        printk("end thread. ret = %d\n" , ret);
        ret = kthread_stop(_task3);
        printk("end thread. ret = %d\n" , ret);
}
module_init(my_start_thread);
module_exit(my_end_thread);

MODULE_AUTHOR("Boring Chen <eric20021232000@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
