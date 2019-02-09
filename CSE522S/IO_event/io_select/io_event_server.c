#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "socket_server_inet.h"

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

	if(create_socket() < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}


	while(read_flag)
	{
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		
		FD_SET(server_fd, &rfds);
		
		retval = select(server_fd + 1, &rfds, NULL, NULL, NULL); // put &tv for a timeout. Otherwise there is no timeout.

		if (retval == -1)
		   perror("select()");
		else
		{

			if(FD_ISSET(STDIN_FILENO, &rfds))
			{
				len = read(STDIN_FILENO, buff, 99);
				if (len > 0)
				{
					printf("Len: %d, Data: %s\n", len, buff);

				}
				else if (len < 0)
				{
					perror("read");
					read_flag = 0;
				}
				else
				{
					printf("Quiting....\n");
					read_flag = 0;
				}
			}
			if(FD_ISSET(server_fd, &rfds))
			{
				if(talk_2_client() < 0)
				{
					printf("Communication problem\n");
					return -1;
				}
				printf("Quiting....\n");
				read_flag = 0;
			}
		}


	}
	   

   exit(EXIT_SUCCESS);
}
