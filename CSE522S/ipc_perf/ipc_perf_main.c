#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h> 
#include <signal.h>

void parent_handler(int rpiSig); //parent handler
void child_handler(int rpiSig); //child handler

int string_to_int(char* a); //convert string to integer
int is_string_digit(char* a); //check if string is a number

int num_comm = 0, child_num_comm = 0;
char* command;

typedef enum {INIT_SIGUSR1_WAIT, INIT_SIGUSR1_RECV, INIT_SIGUSR2_WAIT, INIT_SIGUSR2_RECV, COMM_SIGUSR2_WAIT, COMM_SIGUSR2_RECV} signal_comm;
signal_comm sigParent = INIT_SIGUSR1_WAIT;

int main(int argc, char* argv[])
{
	pid_t childPid;
	//signal handling variables
	struct sigaction rpi_sigs;
	rpi_sigs.sa_handler = parent_handler;
	rpi_sigs.sa_flags = SA_RESTART;
	//signal variables - end
	
	struct timespec tstart, tend; //variables for getting time
	
			
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

	int ret = sigaction(SIGUSR1, &rpi_sigs, NULL); //registering signal handler
	if (ret < 0)
	{
		perror("could not register SIGUSR1");
		return 1;
	}
	ret = sigaction(SIGUSR2, &rpi_sigs, NULL); //registering signal handler
	if (ret < 0)
	{
		perror("could not register SIGUSR2");
		return 1;
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
		
		rpi_sigs.sa_handler = child_handler;
		rpi_sigs.sa_flags = SA_RESTART;
		ret = sigaction(SIGUSR2, &rpi_sigs, NULL);
		//signal variables - end
		
		printf("I am child; Num comm: %d, Command: %s\n", num_comm, command);
		fflush(stdout);
		kill(getppid(),SIGUSR1);

		exit(0);
	}
	else
	{
		//Parent process
		printf("I am parent; Num comm: %d, Command: %s\n", num_comm, command);
		fflush(stdout);
		while(sigParent != INIT_SIGUSR1_RECV);
		clock_gettime(CLOCK_MONOTONIC_RAW, &tstart);
		printf("Starting sec: %ld\n", tstart.tv_sec);

		//sending sigusr2
		printf("Sending sigusr2..\n");
		kill(childPid, SIGUSR2);
		
		if (strcmp(command,"signals") == 0)
		{
			int i = 0;
			while(sigInstance != COMM_SIGUSR2_RECV)
			{
				printf("Command is signal..sending SIGUSR2...%d\n", ++i);
				kill(childPid, SIGUSR2);
			}
		}


	}
	
	return 0;
}


void parent_handler(int rpiSig)
{
	//char* msg = "inside handler\n";
	//write(0, msg, strlen(msg));
	switch(rpiSig)
	{
		case SIGUSR1:
			sigParent = INIT_SIGUSR1_RECV;
			break;
		case SIGUSR2:
			break;
		
		default:
			killpg(getppid(),SIGTERM);
			
	}
	return;	
}

void child_handler(int rpiSig)
{
	//char* msg = "inside handler\n";
	//write(0, msg, strlen(msg));
	switch(rpiSig)
	{
		case SIGUSR1:
			sigParent = INIT_SIGUSR1_RECV;
			break;
		case SIGUSR2:
			break;
		
		default:
			killpg(getppid(),SIGTERM);
			
	}
	return;	
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


