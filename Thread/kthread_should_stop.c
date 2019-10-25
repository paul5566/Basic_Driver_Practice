struct kthread {

       int should_stop;

       struct completion exited;

};

int kthreadd(void *unused)
{

       struct task_struct *tsk = current;


       /* Setup a clean context for our children to inherit. */

       set_task_comm(tsk, "kthreadd");

       ignore_signals(tsk);

       set_cpus_allowed_ptr(tsk, cpu_all_mask);

       set_mems_allowed(node_states[N_HIGH_MEMORY]);


       current->flags |= PF_NOFREEZE | PF_FREEZER_NOSIG;

 
       for (;;) {

              set_current_state(TASK_INTERRUPTIBLE);

              if (list_empty(&kthread_create_list))

                     schedule();

              __set_current_state(TASK_RUNNING);

 
              spin_lock(&kthread_create_lock);

              while (!list_empty(&kthread_create_list)) {

                     struct kthread_create_info *create;

                     create = list_entry(kthread_create_list.next,

                                       struct kthread_create_info, list);

                     list_del_init(&create->list);

                     spin_unlock(&kthread_create_lock);

 

                     create_kthread(create);

 

                     spin_lock(&kthread_create_lock);

              }

              spin_unlock(&kthread_create_lock);

       }

       return 0;
}

/**
 * kthread_stop - stop a thread created by kthread_create().
 * @k: thread created by kthread_create().
 *
 *    
 *
 *	Sets kthread_should_stop() for @k to return true, wakes it, and
 *
 *	waits for it to exit. This can also be called after kthread_create()
 *
 *	instead of calling wake_up_process(): the thread will exit without
 *
 *	calling threadfn().
 *
 *
 *
 * If threadfn() may call do_exit() itself, the caller must ensure
 *
 *	task_struct can't go away.
 *
 *
 *
 *	Returns the result of threadfn(), or %-EINTR if wake_up_process()
 *
 *  was never called.
 *
 *               */

int kthread_stop(struct task_struct *k)
{

       struct kthread *kthread;

       int ret;


       trace_sched_kthread_stop(k);

       get_task_struct(k);


       kthread = to_kthread(k);

       barrier(); /* it might have exited */

       if (k->vfork_done != NULL) {

              kthread->should_stop = 1;

              wake_up_process(k);

              wait_for_completion(&kthread->exited);

       }

       ret = k->exit_code;

       put_task_struct(k);

       trace_sched_kthread_stop_ret(ret);

       return ret;

}

