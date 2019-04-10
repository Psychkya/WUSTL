/* 14.3_tak_module.c
 522 Studio 14 q 3
 based on: https://www.cse.wustl.edu/~cdgill/courses/cse422/studios/simple_module.c
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h> // module_param
#include <linux/string.h> // strcmp
#include <linux/fs_struct.h> // priority struct
#include <linux/hrtimer.h> // hrtimer
#include <linux/kthread.h> // thread
#include <linux/ktime.h> // ktimer, ktime_t
#include <linux/sched.h> //wake_up_process, sched_setscheduler
#include <uapi/linux/sched/types.h> // sched_param

static char * sched_type = "OTHER"; // scheduler mode; OTHER (default), FIFO, RR
static int core; // core # on which to run
static unsigned long period; // period in ms
static int iterations; // # iterations

static struct task_struct * task; // ptr to kernel thread task CPU
static struct sched_param param; // sched param

static ktime_t start, end, interval, for_timer; // time interval vars
static struct hrtimer timer; // timer

module_param(sched_type, charp, 0); // module parameter
module_param(core, int, 0); // module parameter
module_param(period, int, 0); // module parameter
module_param(iterations, int, 0); // module parameter

/* timer expiration function - wakes up thread */
enum hrtimer_restart timer_expiration(struct hrtimer * param_timer) {
  printk(KERN_INFO "in timer_expiration function\n");
  /* wake up kthread */
  wake_up_process(task);
  return HRTIMER_NORESTART; // timer is one-time—is this correct Nayan?
}

/* task function - calls ktime_get() as many times as specified for given subtask */
static void task_loop(int iterations) {
  int i;
	start = ktime_get();
  for (i = 0; i < iterations; i++) {
		ktime_get();
  }
	finish = ktime_get();
    /*subtrace finish from start and convert to nanosecond; compare with period*/
    if(ktime_to_ns(ktime_sub(finish, start)) < period) {
        //if period is greater, set a ktime_t value for period - interval and start timer
        for_timer = ktimeset(0, (period - ktime_to_ns(ktime_sub(finish, start))));
        hrtimer_start(&timer, for_timer, HRTIMER_MODE_ABS);
    }
    else {
        //else start timer for 0 value so it wakes up thread immediately
        hrtimer_start(&timer, 0, HRTIMER_MODE_ABS);
    }

	printk(KERN_INFO "leaving task_loop function\n");
}

/* init function - logs that initialization happened, returns success */
static int mod_init (void) {
	/* create kthread */
  if ((task = kthread_create(task_loop, (void *) &core, "0_kthread")) == ERR_PTR(-ENOMEM)) {
    printk(KERN_ALERT "Error: kthread_create task\n");
    return -1;
  }

	/* bind kthreads to core */
  if (task) {
    kthread_bind(task, &core);
  }

  /* set sched policy and priority */
	if (strcmp(sched_type, "OTHER") == 0) {
		/* scheduler: OTHER */
		printk(KERN_INFO "scheduler: OTHER\n");

		/* set priority (must be 0) */
		param.sched_priority = 0;
		/* set scheduling class */
		if (sched_setscheduler(task, SCHED_OTHER, &param) == -1) {
			printk(KERN_ALERT "Error: set scheduler\n");
			return -1;
		}
	} else if (strcmp(sched_type, "FIFO") == 0) {
		/* scheduler: FIFO */
		printk(KERN_INFO "scheduler: FIFO\n");

		/* set priority (must be between 1 and 99) */
		param.sched_priority = 50;
		/* set scheduling class */
		if (sched_setscheduler(task, SCHED_FIFO, &param) == -1) {
			printk(KERN_ALERT "Error: set scheduler\n");
			return -1;
		}
	} else if (strcmp(sched_type, "RR") == 0) {
		/* scheduler: RR */
		printk(KERN_INFO "scheduler: RR\n");

		/* set priority (must be between 1 and 99) */
		param.sched_priority = 50;
		/* set scheduling class */
		if (sched_setscheduler(task, SCHED_RR, &param) == -1) {
			printk(KERN_ALERT "Error: set scheduler\n");
			return -1;
		}
	}

	if (period == 0) {
		/* immediately invoke its task function again as soon as it completes */
	} else {
		/* set timer (<period> ms) */
    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS); // create timer
    timer.function = &timer_expiration;

    /* wake up process */
    wake_up_process(task);

		/* track when the task function was last invoked, set a timer for the end of the current period, and suspend itself */
		// hrtimer_start(&timer, period, HRTIMER_MODE_ABS);
        hrtimer_start(&timer, 0, HRTIMER_MODE_ABS); //for the first time immediately wake it up

		/* timer goes off it should resume the kernel thread which should then invoke the task function */
	}
	printk(KERN_ALERT "module initialized\n");
	return 0;
}

/* exit function - logs that the module is being removed */
static void mod_exit (void) {
	printk(KERN_INFO " time to run: %d\n iterations: %d", ktime_to_ns(end) - ktime_to_ns(start), iterations);
	printk(KERN_ALERT "module is being unloaded\n");
}

module_init (mod_init);
module_exit (mod_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Elaine Cole, Nayan Saikia");
MODULE_DESCRIPTION ("522 Studio 14");
