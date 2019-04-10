
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>

static ulong period_sec = 0;
module_param(period_sec, ulong, 0644);

static ulong period_nsec = 1000000000;
module_param(period_nsec, ulong, 0644);

static ktime_t time_int_var;
static struct hrtimer hr_timer_var;

static struct task_struct *thread_var1;
static struct task_struct *thread_var2;
static struct task_struct *thread_var3;
struct sched_param sp1, sp2, sp3;
int counter = 0;

static int func_thread(void * loopVal)
{
	int i, loop;
	loop = (int)loopVal;
	while(!kthread_should_stop())
	{
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		printk(KERN_INFO "And I awoke and found me here...On the cold Kernel's side. Param nsec: %lu", period_nsec);
		for (i = 0; i < loop; i++) 
		{
			ktime_get();
		}
	}
	return 0;
}

enum hrtimer_restart timer_function(struct hrtimer *time_restart)
{
	wake_up_process(thread_var1);
	if (counter == 1) {
		wake_up_process(thread_var2);
	}
	if (counter == 2) {
		wake_up_process(thread_var2);
		wake_up_process(thread_var3);
		counter = 0;
	}
	counter++;
	hrtimer_forward(time_restart, ktime_get(), time_int_var);
	return HRTIMER_RESTART;
}

static int rt_timer_init(void)
{
	int loopVal;
	time_int_var = ktime_set(period_sec, period_nsec);
	hrtimer_init(&hr_timer_var, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer_var.function = &timer_function;
	loopVal = 2000000;
	thread_var1 = kthread_create(func_thread,(void*)loopVal,"first_thread");
	if(!thread_var1)
	{
		printk(KERN_ERR "Alas...I failed to exist...thread_var1\n");
	}

	loopVal = 4000000;
	thread_var2 = kthread_create(func_thread,(void*)loopVal,"second_thread");
	if(!thread_var2)
	{
		printk(KERN_ERR "Alas...I failed to exist...thread_var2\n");
	}

	loopVal = 6000000;
	thread_var3 = kthread_create(func_thread,(void*)loopVal,"third_thread");
	if(!thread_var3)
	{
		printk(KERN_ERR "Alas...I failed to exist...thread_var3\n");
	}

	sp1.sched_priority = 60;
	sp2.sched_priority = 40;
	sp3.sched_priority = 10;
	printk(KERN_INFO "I EXIST! but dormant...\n");
	kthread_bind(thread_var1, 1); //bind to CPU 1
	kthread_bind(thread_var2, 1); //bind to CPU 2
	kthread_bind(thread_var3, 1); //bind to CPU 2
	sched_setscheduler(thread_var1, SCHED_FIFO, &sp1);
	sched_setscheduler(thread_var2, SCHED_FIFO, &sp2);
	sched_setscheduler(thread_var3, SCHED_FIFO, &sp3);
	wake_up_process(thread_var1);
	wake_up_process(thread_var2);
	wake_up_process(thread_var3);
	hrtimer_start(&hr_timer_var, time_int_var, HRTIMER_MODE_REL);
	
    return 0;
}

static void rt_timer_exit(void)
{
	int ret;
    printk(KERN_ALERT "rt_timer is being unloaded\n");
	ret = hrtimer_cancel(&hr_timer_var);
	if (ret) {
		printk(KERN_ALERT "Timer still in use\n");
	}
	ret = kthread_stop(thread_var1);
	if (ret) {
		printk(KERN_ALERT "Could not stop thread1\n");
	}
	ret = kthread_stop(thread_var2);
	if (ret) {
		printk(KERN_ALERT "Could not stop thread2\n");
	}
	ret = kthread_stop(thread_var3);
	if (ret) {
		printk(KERN_ALERT "Could not stop thread3\n");
	}	
}

module_init(rt_timer_init);
module_exit(rt_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Studio 8");
MODULE_DESCRIPTION ("RT timer example");
