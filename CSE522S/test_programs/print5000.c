#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define size 50000

volatile int i;

int called [size];

void sigint_handler( int signum ){

	called[i] = 1;
	char* msg = "inside handler\n";
	write(0, msg, strlen(msg));
	return;
}

int main (int argc, char* argv[]){

	struct sigaction ss;
	
	//The following lines of code was to introduce a delay
	//struct timespec t1, t2;
	//t1.tv_sec = 0;
	//t1.tv_nsec = 50000;

	ss.sa_handler = sigint_handler;
	ss.sa_flags = SA_RESTART;

	sigaction( SIGINT, &ss, NULL );

	for(i = 0; i < size; i++){
		printf("inside main i: %d\n", i);
		//The if block is to check what gets displayed when ctrl+c is pressed
		if(called[i] == 1)
		{
			break;
		}
		//nanosleep(&t1, &t2); //Sleep for 50000 ns
	}

	
	for(i = 0; i < size; i++){
		if( called[i] == 1 )
			printf("%d was possibly interrupted\n", i);
	}
	

	return 0;
}
