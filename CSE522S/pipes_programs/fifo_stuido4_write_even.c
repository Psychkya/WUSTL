#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>


int main(void)
{
	char* fifo_file = "/home/pi/WUSTL/CSE522S/pipes_programs/fifotest";
	FILE* wfile;

	//The following lines of code was to introduce a delay
	struct timespec t1, t2;
	t1.tv_sec = 0;
	t1.tv_nsec = 500000;

	for (int i = 1; i < 1000 ; i++)
	{
		if (i % 2 == 0)
		{
			wfile = fopen(fifo_file, "w");
			if (wfile == NULL)
			{
				perror("Error opening fifo file");
				exit(1);
			}			
			fprintf(wfile, "%d\n", i);
			fclose(wfile);
		} 
		nanosleep(&t1, &t2); //Sleep for 50000 ns
	}
	return 0;
}

