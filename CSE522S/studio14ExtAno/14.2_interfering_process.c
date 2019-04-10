/*
	14.2_interfering_process.c
	Studio 14 q 2
*/

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h> // sched_setscheduler
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/time.h> // timeval, timeout
#include <pthread.h>

const int num_expected_args = 5;

int is_string_digit(char * a);
int string_to_int(char * a);
void timespec_sub(struct timespec * start, struct timespec * stop, struct timespec * interval);

struct timespec start, finish, interval, for_sleep;
struct timeval tv;
// http://pubs.opengroup.org/onlinepubs/7908799/xsh/timer_settime.html


/* task function - calls clock_gettime() as many times as specified for given subtask */
static void task_loop(int iterations) {
  int i;
  for (i = 0; i < iterations; i++) {
    clock_gettime(CLOCK_MONOTONIC, &finish);
  }
}

int main(int argc, char * argv[]) {
	cpu_set_t set;
  unsigned long cpu_val;
	char * sched_type;
	struct sched_param param;
	int period, iterations;
    int ret;

  if (argc != num_expected_args) {
    printf("Usage: ./interfering_process <scheduler type> <core number> <period> <number of iterations>\n");
    return -1;
  }

  CPU_ZERO(&set);

	/* fetch number of core to run on */
	if (is_string_digit(argv[2])) {
		cpu_val = string_to_int(argv[2]);
		if (cpu_val > 3) {
			printf("error: core ID cannot be greater than 3\n");
			return -1;
		}
		CPU_SET(cpu_val, &set);
	} else {
		printf("error: core ID must be number\n");
		return -1;
	}

	/* set CPU affinity */
	if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) < 0) {
			perror("error: sched_setaffinity\n");
			return -1;
	}

	/* handle scheduler type */
	sched_type = argv[1];
	if (strcmp(sched_type, "OTHER") == 0) {
		/* scheduler: OTHER */
		printf("sched OTHER\n");

		/* set priority (must be 0) */
		param.sched_priority = 0;
		/* set scheduling class */
		if (sched_setscheduler(getpid(), SCHED_OTHER, &param) == -1) {
			perror("error: sched_setscheduler\n");
			return -1;
		}
	} else if (strcmp(sched_type, "FIFO") == 0) {
		/* scheduler: FIFO */
		printf("sched FIFO\n");

		/* set priority (must be between 1 and 99) */
		param.sched_priority = 50;
		/* set scheduling class */
		if (sched_setscheduler(getpid(), SCHED_FIFO, &param) == -1) {
			perror("error: sched_setscheduler\n");
			return -1;
		}
	} else if (strcmp(sched_type, "RR") == 0) {
		/* scheduler: RR */
		printf("sched RR\n");

		/* set priority (must be between 1 and 99) */
		param.sched_priority = 50;
		/* set scheduling class */
		if (sched_setscheduler(getpid(), SCHED_RR, &param) == -1) {
			perror("error: sched_setscheduler\n");
			return -1;
		}
	} else {
		printf("error: scheduler type must be either OTHER, FIFO, or RR\n");
		return -1;
	}

    if(is_string_digit(argv[3])){
        period = string_to_int(argv[3]);
    } else {
        printf("error: period entered is not numeric\n");
        return -1;
    }

    if (is_string_digit(argv[4])) {
        iterations = string_to_int(argv[4]);
    } else {
        printf("error: iterations is not numeric\n");
        return -1;
    }


	if (period == 0) {
		/* invoke task function */
		printf("period is 0\n");
        while(1)
        {
            task_loop(iterations);
        }

	} else {
		printf("period not 0\n");
		/* track when task function was last invoked */
        while(1){
            clock_gettime(CLOCK_MONOTONIC, &start); // timestamp @ start
            task_loop(iterations);
            clock_gettime(CLOCK_MONOTONIC, &finish);
            timespec_sub(&start, &finish, &interval);
            if ((interval.tv_sec * 1000000000 + interval.tv_nsec) < period) {
                printf("sleeping...\n");
                for_sleep.tv_sec = 0;
                for_sleep.tv_nsec = period - (interval.tv_sec * 1000000000 + interval.tv_nsec);
                nanosleep(&for_sleep, (struct timespec *)NULL);
            }
            /* set timer for end of curr period */
            /* suspend itself */

            /* timer done; resume thread (invoking task function) */
        }
	}
  return 0;
}

void timespec_sub(struct timespec * start, struct timespec * stop, struct timespec * interval) {
    interval->tv_nsec = stop->tv_nsec - start->tv_nsec;
    if(interval->tv_nsec < 0) {
        interval->tv_nsec += 1000000000;
        interval->tv_sec = stop->tv_sec - start->tv_sec - 1;
    }
    else {
        interval->tv_sec = stop->tv_sec - start->tv_sec;
    }


}

int is_string_digit(char* a) {
	int result = -1;
	int len = strlen(a);
	for (int i = 0; i < len; i++) {
		if (a[i] >= '0' && a[i] <= '9') {
			result = 1;
		} else {
			result = 0;
			break;
		}
	}
	return result;
}

int string_to_int(char* a) {
	int result = 0;
	int len = strlen(a);
	for (int i = 0; i < len; i++) {
		result = result * 10 + (a[i] - '0');
	}
	return result;
}