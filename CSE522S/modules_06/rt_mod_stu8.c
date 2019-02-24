
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


static ulong period_sec = 1;
module_param(period_sec, ulong, 0666);

static ulong period_nsec = 1;
module_param(period_nsec, ulong, 0666);

static ktime_t time_int_var;
static struct hrtimer hr_timer_var;

static struct task_struct *thread_var;



static int rt_timer_init(void)
{
	time_int_var = ktime_set(period_sec, period_nsec);
	hrtimer_init(&hr_timer_var, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer_var.function = &some_callbac_function; //YET TO BE DEFINED!
	
	thread_var = kthread_create(some_function,NULL,"rt_timer_thread");
	if(thread_var)
	{
		printk(KERN_INFO "I EXIST! but dormant...\n");
		wake_up_process(thread_var);
		hrtimer_start(&hr_timer_var, time_int_var, HRTIMER_MODE_REL);
	}
	else
	{
		printk(KERN_ERR "Alas...I failed to exist...\n");
	}
	
    return 0;
}

static int func_thread()
{
	while(!kthread_should_stop())
	{
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		printk(KERN_INFO "And I awoke and found me here...On the cold Kernel's side");
	}
	return 0;
}

static void rt_time_exit(void)
{
    printk(KERN_ALERT "simple module is being unloaded\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("LKD Chapter 17");
MODULE_DESCRIPTION ("Simple CSE 422 Module Template");
