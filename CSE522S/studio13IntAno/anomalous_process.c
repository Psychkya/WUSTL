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

const int num_expected_args = 3;

int main(int argc, char* argv[]) {

	char * slowbranch;
	unsigned int wait_sec;

	if (argc > num_expected_args || argc == 1) {
		printf("Usage: ./anomalous_process <slowbranch> <sec to wait>\n");
		exit(-1);
	}
	if (strcmp(argv[1], "slowbranch") != 0) {
		printf("Usage: \"slowbranch\" not received\n");
		exit(-1);
	}
	slowbranch = argv[1];

	/* convert each arg into unsigned long int */
	if ((wait_sec = atoi(argv[2])) == 0) {
		printf("Error: no valid conversion could be performed\n");
		exit(-1);
	}

	while(1) {
		sleep(wait_sec);
		fork();
	}

	return 0;
}
