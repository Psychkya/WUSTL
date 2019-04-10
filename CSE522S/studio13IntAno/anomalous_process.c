/*
	13.3_anomalous_process.c
	Studio 13 q 3
*/

#include <stdio.h>  // printf
#include <stdlib.h> // exit, atoi, strtoul
#include <unistd.h>
#include <string.h>
#include <errno.h> // error
#include<sys/wait.h> // wait
#include <sys/types.h> // wait
#include <time.h>

const int num_expected_args = 5;

void print_usage();
int is_string_digit(char * a);
int string_to_int(char * a);

int main(int argc, char* argv[]) {

	char * slowbranch;
	unsigned int wait_sec;
	int lower = 1;
    int upper = 100;
	unsigned int fork_probability;
	unsigned int term_probability;
	unsigned probability;

	if (argc > num_expected_args || argc == 1) {
		printf("Too few or too many arguments\n");
		print_usage();
		exit(-1);
	}
	if (strcmp(argv[1], "slowbranch") != 0 && strcmp(argv[1], "randombranch") != 0) {
		printf("Usage: \"slowbranch\" or \"randombranch\" not received\n");
		print_usage();
		exit(-1);
	}

	if (strcmp(argv[1], "slowbranch") == 0) {
		/* convert each arg into unsigned long int */
		if (is_string_digit(argv[2])) {
			wait_sec = string_to_int(argv[2]);
		}
		else {
			printf("Error: no valid conversion could be performed\n");
			print_usage();
			exit(-1);
		}

		while(1) {
			sleep(wait_sec);
			fork();
		}
	}

	if(strcmp(argv[1], "randombranch") == 0) {
		if (argc < num_expected_args ){
			printf("Too few arguments for randombranch\n");
			print_usage();
			exit(-1);			
		}
		
		if (is_string_digit(argv[2])) {
			wait_sec = string_to_int(argv[2]);
		}
		else {
			printf("Error: no valid conversion of time could be performed\n");
			print_usage();
			exit(-1);
		}

		if (is_string_digit(argv[3])) {
			fork_probability = string_to_int(argv[3]);
		}
		else {
			printf("Error: no valid conversion of fork probability could be performed\n");
			print_usage();
			exit(-1);
		}

		if (is_string_digit(argv[4])) {
			term_probability = string_to_int(argv[4]);
		}
		else {
			printf("Error: no valid conversion of terminate probability could be performed\n");
			print_usage();
			exit(-1);
		}		

		srand(time(0));

		while(1) {
			sleep(wait_sec);
			probability = rand() % ((upper - lower) + 1)  + lower;
			if ( probability <= fork_probability) {
				fork();
			}
			probability = rand() % ((upper - lower) + 1)  + lower;
			if ( probability <= term_probability) {
				exit(1);
			}

		}

	}

	return 0;
}

void print_usage()
{
	printf("Usage: ./anomalous_process <[slowbranch][randombranch]> <sec to wait> <[probability of fork if randombranch]> <[probability to terminate if randombranch]>\n");
	printf("For randombranch, the last two options should be integer between 1 and 100, inclusive\n");
	printf("For slowbranch, last two options are ignore if entered\n");
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