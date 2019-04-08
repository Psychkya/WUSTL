#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <time.h>

int is_string_digit(char * a);
int string_to_int(char * a);

int main(int argc, char * argv[])
{
    cpu_set_t setcpu;
    char display_buffer[1024];
    int lower = 1;
    int upper = 100;
    int result = 0;

    unsigned long cpu_val;

    if (argc < 2 || argc > 5)
    {
        printf("Incorrect number of arguments - should be between 1 and 4.\n");
        printf("Usage: core_user_process [0] [1] [2] [3]\n");
        return -1;
    }

    CPU_ZERO(&setcpu);
    // sprintf(display_buffer, "%s", argv[0]);
    strcpy(display_buffer, argv[0]); //store arguments to display

    for (int i = 1; i < argc; i++)
    {
        if (is_string_digit(argv[i]))
        {
            cpu_val = string_to_int(argv[i]);
            if (cpu_val > 3)
            {
                printf("CPU core id cannot be greater than 3\n");
                printf("Usage: core_user_process [0] [1] [2] [3]\n");
                return -1;
            }
            CPU_SET(cpu_val, &setcpu);
            strcat(display_buffer, " ");
            strcat(display_buffer, argv[i]); //store arguments to display
        }
        else
        {
            printf("One of the argument is not numeric\n");
            printf("Usage: core_user_process [0] [1] [2] [3]\n");
            return -1;
        }

    }

    if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &setcpu) < 0)
    {
        perror("sched_setaffinity()");
        return -1;
    }

    printf("Process: %d Arguments: %s\n", getpid(), display_buffer);

    srand(time(0));

    while(1)
    {
        result = ((rand() % (upper - lower) + 1) + lower ) * ((rand() % (upper - lower) + 1) + lower );
    }
    
    return 0;


}

int is_string_digit(char* a)
{
	int result = -1;
	int len = strlen(a);
	for (int i = 0; i < len; i++)
	{
		if(a[i] >= '0' && a[i] <= '9')
		{
			result = 1;
		}
		else
		{
			result = 0;
			break;
		}
	}
	return result;

}

int string_to_int(char* a)
{
	int result = 0;
	int len = strlen(a);
	for (int i = 0; i < len; i++)
	{
		result = result * 10 + (a[i] - '0');
	}
	return result;
}