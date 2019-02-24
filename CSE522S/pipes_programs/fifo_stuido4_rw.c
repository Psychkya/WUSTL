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
FILE* wfile;

int main(void)
{
	int var, d_var;
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
	
	//char read_buff[100];
	if (mkfifo(fifo_file,0666) < 0)
	{
		perror("Error creating fifo");
		return 1;
	}

	rfile = fopen(fifo_file, "r");
	wfile = fopen(fifo_file, "w");
	if (rfile == NULL)
	{
		perror("Error opening fifo file");
		exit(1);
	}
	fscanf(rfile, "%d", &var);
	
	fclose(rfile);
	rfile = NULL; //prevent from closing already closed fd after ctrl+c is pressed
	d_var = var * 2;
	printf("Original value: %d, Doubled value: %d \n", var, d_var);
	unlink(fifo_file);
	return 0;
}

void rpi_signal_handler(int rpiSig)
{
	if (rfile != NULL)
	{
		fclose(rfile); //close fd if it is not already closed
	}
	//fclose(wfile);
	unlink(fifo_file); //unlink the file before exiting
	if (raise(SIGTERM) < 0)
	{
		perror("Error handling SIGTERM");
		exit(1);
	}
	return;	
}


