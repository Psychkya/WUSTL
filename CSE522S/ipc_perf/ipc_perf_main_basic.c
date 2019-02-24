#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h> 
#include <signal.h>

int string_to_int(char* a); //convert string to integer
int is_string_digit(char* a); //check if string is a number

unsigned int num_comm;
char* command;

int main(int argc, char* argv[])
{
	pid_t childPid;
			
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

	printf("Parent passing arguments to child..number of communication:%d, command:%s\n", num_comm, command);
	fflush(stdout);

	childPid = fork();
	
	if (childPid < 0)
	{
		perror("Fork failed");
		return 1;
	}
	else if(childPid == 0)
	{
		//Child process
		
		printf("I am child; number of communication from parent: %d, Command from parent: %s\n", num_comm, command);
		fflush(stdout);

		exit(0);
	}
	else
	{
		//Parent process
		//Do nothing	

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


