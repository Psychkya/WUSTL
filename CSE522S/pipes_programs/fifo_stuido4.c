#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

void rpi_signal_handler(int rpiSig); //function definition
char* fifo_file = "/home/pi/WUSTL/CSE522S/pipes_programs/fifotest";
FILE* rfile;

int main(void)
{
	//signal handling variables
	struct sigaction rpi_sigs;
	rpi_sigs.sa_handler = rpi_signal_handler;
	rpi_sigs.sa_flags = SA_RESTART;
	int ret = sigaction(SIGINT, &rpi_sigs, NULL); //registering signal handler
	if (ret < 0)
	{
		perror("could not register sigaction");
		return 1;
	}
	//signal handling - end
	
	char read_buff[100];
	if (mkfifo(fifo_file,0666) < 0)
	{
		perror("Error creating fifo");
		return 1;
	}

	while(1)
	{
		rfile = fopen(fifo_file, "r");
		if (rfile == NULL)
		{
			perror("Error opening fifo file");
			exit(1);
		}
		while(fgets(read_buff, sizeof(read_buff), (FILE*)rfile)	!= NULL)
		{
			printf("%s", read_buff);
		}
		fclose(rfile);
		rfile = NULL; //prevent from closing already closed fd after ctrl+c is pressed
	}

	return 0;
}

void rpi_signal_handler(int rpiSig)
{
	if (rfile != NULL)
	{
		fclose(rfile); //close fd if it is not already closed
	}
	unlink(fifo_file); //unlink the file before exiting
	if (raise(SIGTERM) < 0)
	{
		perror("Error handling SIGTERM");
		exit(1);
	}
	return;	
}


