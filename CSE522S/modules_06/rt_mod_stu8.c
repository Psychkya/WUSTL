
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>

static ulong period_sec = 1;
module_param(period_sec, ulong, 0644);

static ulong period_nsec = 1;
module_param(period_nsec, ulong, 0644);

static ktime_t time_int_var;
static struct hrtimer hr_timer_var;

static struct task_struct *thread_var;
struct sched_param sp;

static int func_thread(void)
{
	int i, loop;
	while(!kthread_should_stop())
	{
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		printk(KERN_INFO "And I awoke and found me here...On the cold Kernel's side");
		loop = (period_sec * 1000000000 + period_nsec) / 2;
		for (i = 0; i < loop; i++) 
		{
			ktime_get();
		}
	}
	return 0;
}

enum hrtimer_restart timer_function(struct hrtimer *time_restart)
{
	wake_up_process(thread_var);
	hrtimer_forward(time_restart, ktime_get(), time_int_var);
	return HRTIMER_RESTART;
}

static int rt_timer_init(void)
{
	time_int_var = ktime_set(period_sec, period_nsec);
	hrtimer_init(&hr_timer_var, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer_var.function = &timer_function;
	sp.sched_priority = 1;
	
	thread_var = kthread_create(func_thread,NULL,"rt_timer_thread");
	if(thread_var)
	{
		printk(KERN_INFO "I EXIST! but dormant...\n");
		kthread_bind(thread_var, 1); //bind to CPU 1
		sched_setscheduler(thread_var, SCHED_FIFO, &sp);
		wake_up_process(thread_var);
		hrtimer_start(&hr_timer_var, time_int_var, HRTIMER_MODE_REL);
	}
	else
	{
		printk(KERN_ERR "Alas...I failed to exist...\n");
	}
	
    return 0;
}

static void rt_timer_exit(void)
{
	int ret;
    printk(KERN_ALERT "rt_timer is being unloaded\n");
	ret = hrtimer_cancel(&hr_timer_var);
	if (ret) printk(KERN_ALERT, "Timer still in use\n");
	ret = kthread_stop(thread_var);
	if (ret) printk(KERN_ALERT, "Could not stop thread\n");
}

module_init(rt_timer_init);
module_exit(rt_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Studio 8");
MODULE_DESCRIPTION ("RT timer example");
