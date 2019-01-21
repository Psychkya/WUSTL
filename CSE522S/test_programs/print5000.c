#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

	ss.sa_handler = sigint_handler;
	ss.sa_flags = SA_RESTART;

	sigaction( SIGINT, &ss, NULL );

	for(i = 0; i < size; i++){
		printf("inside main i: %d\n", i);
		if(called[i] == 1)
		{
			break;
		}
	}

	
	for(i = 0; i < size; i++){
		if( called[i] == 1 )
			printf("%d was possibly interrupted\n", i);
	}
	

	return 0;
}
