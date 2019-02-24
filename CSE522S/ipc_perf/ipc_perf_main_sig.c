#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h> 
#include <signal.h>
#define NANO_SEC 1E9

void parent_handler_SIGUSR1(int rpiSig); //parent handler
void parent_handler_SIGUSR2(int rpiSig); //parent handler
void child_handler_SIGUSR2(int rpiSig); //child handler

int string_to_int(char* a); //convert string to integer
int is_string_digit(char* a); //check if string is a number

int num_comm = 0;
char* command;
int count_c_usr2 = 0;

typedef enum {INIT_SIGUSR1_WAIT, INIT_SIGUSR1_RECV, INIT_SIGUSR2_WAIT, INIT_SIGUSR2_RECV} signal_comm;
signal_comm sigParent = INIT_SIGUSR1_WAIT;


int main(int argc, char* argv[])
{
	pid_t childPid;
	int count_p_usr2 = 0;
	double elapsed_time = 0;
	
	struct timespec tstart, tend; //variables for getting time

	struct sigaction rpi_sigs_pusr1;
	struct sigaction rpi_sigs_pusr2;
	
			
	if (argc != 3)
	{
		printf("Incorrect arguments..\nUsage: ipc_perf_main <number of communication> <communication method>\nNumber of communication should be unsigned integer\n");
		return 0;
	} 
	else
	{
		if ( is_string_digit(argv[1]) < 0)
		{
			printf("Number of communication is not an unsigned integer\nUsage: ipc_perf_main <number of communication> <communication method>\n");
			return 0;
		}
		else
		{
			num_comm = string_to_int(argv[1]);
			command = argv[2];
		}
	}

	if(strcmp(command, "signals") == 0)
	{
		//Register signals
		rpi_sigs_pusr1.sa_handler = parent_handler_SIGUSR1;
		rpi_sigs_pusr1.sa_flags = SA_RESTART;
		int ret = sigaction(SIGUSR1, &rpi_sigs_pusr1, NULL); //registering signal handler
		if (ret < 0)
		{
			perror("could not register parent SIGUSR1");
			return -1;
		} 

		rpi_sigs_pusr2.sa_handler = parent_handler_SIGUSR2;
		rpi_sigs_pusr2.sa_flags = SA_RESTART;
		ret = sigaction(SIGUSR2, &rpi_sigs_pusr2, NULL); //registering signal handler	
		if (ret < 0)
		{
			perror("could not register parent SIGUSR2");
			return -1;
		} 			
	}

	childPid = fork();
	
	if (childPid < 0)
	{
		perror("Fork failed");
		return 1;
	}
	else if(childPid == 0)
	{
		//Child process
		
		//register signal handler
		struct sigaction rpi_sigs_cusr2;
		rpi_sigs_cusr2.sa_handler = child_handler_SIGUSR2;
		rpi_sigs_cusr2.sa_flags = SA_RESTART;
		int ret = sigaction(SIGUSR2, &rpi_sigs_cusr2, NULL); //registering signal handler			

		//send sigusr1 to parent
		kill(getppid(), SIGUSR1);
		while(count_c_usr2 < num_comm);
		kill(getppid(), SIGUSR2);
		exit(0);
	}
	else
	{
		//Parent process
		while(sigParent != INIT_SIGUSR1_RECV);
		printf("Sigusr1 received from child\n");
		clock_gettime(CLOCK_MONOTONIC_RAW,&tstart);
		sigParent = INIT_SIGUSR2_WAIT;
		while(sigParent != INIT_SIGUSR2_RECV)
		{
			kill(childPid, SIGUSR2);
			count_p_usr2++;
		}
		clock_gettime(CLOCK_MONOTONIC_RAW,&tend);
		elapsed_time = (tend.tv_sec - tstart.tv_sec) * NANO_SEC + (tend.tv_nsec - tstart.tv_nsec);	
		printf("Sigusr2 received from child\nElapsed Time:%f\n", elapsed_time);

	}
	
	return 0;
}

void parent_handler_SIGUSR1(int rpiSig)
{
	sigParent  = INIT_SIGUSR1_RECV;
}


void parent_handler_SIGUSR2(int rpiSig)
{
	sigParent = INIT_SIGUSR2_RECV;
}

void child_handler_SIGUSR2(int rpiSig)
{
	count_c_usr2++;
}

int is_string_digit(char* a)
{
	int result = -1;
	int len = strlen(a);
	for (int i = 0; i < len; i++)
	{
		if(a[i] >= '0' && a[i] <= '9')
		{
			result = 0;
		}
		else
		{
			result = -1;
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


