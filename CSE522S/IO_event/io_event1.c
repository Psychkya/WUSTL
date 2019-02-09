#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	fd_set rfds;

	/* Wait up to five seconds. This is for timeout */
	//struct timeval tv;
	//tv.tv_sec = 5;
	//tv.tv_usec = 0;

	int retval;
	int read_flag = 1;
	char buff[100];
	int len;



   /* Watch stdin (fd 0) to see when it has input. */


	while(read_flag)
	{
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		
		retval = select(1, &rfds, NULL, NULL, NULL); // put &tv for a timeout. Otherwise there is no timeout.

		if (retval == -1)
		   perror("select()");
		else
		{

			len = read(STDIN_FILENO, buff, 99);
			if (len > 0)
			{
				printf("Len: %d, Data: %s\n", len, buff);
				for (int i = 0; i < len; i++)
				{
					if (buff[i] == 4)
					{
						read_flag = 0;
						break;
					}
				}
			}
			else if (len < 0)
			{
				perror("read");
				read_flag = 0;
			}
			else
			{
				printf("Quiting....\n");
				fflush(stdout);
				read_flag = 0;
			}
		}


	}
	   
  
	   

   exit(EXIT_SUCCESS);
}
