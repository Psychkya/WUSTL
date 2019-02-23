/* USING POLL*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include "socket_server_inet.h"
#define DELIMITER '\n'


int main(void)
{
	struct pollfd fds[3];


	int retval;
	int read_flag = 1;
	char buff[100];
	unsigned int malloc_len = 0;
	char* store_buff = NULL;
	int numfds = 0;

	if(create_socket() < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	numfds++;
	fds[1].fd = server_fd;
	fds[1].events = POLLIN;
	numfds++;
	
	while(read_flag)
	{
		
		retval = poll (fds, numfds, -1); //poll indefinitely

		if (retval == -1)
		{
		   perror("poll()");
		   return retval;
	    }
		else
		{

			if(fds[0].revents & POLLIN)
			{
				int ret = read(STDIN_FILENO, buff, 99);
				if (ret > 0)
				{
					printf("Len: %d, Data: %s\n", ret, buff);

				}
				else if (ret < 0)
				{
					perror("read");
					read_flag = 0;
				}
				else
				{
					fds[0].fd = -1;
					//numfds--;
					printf("Removing stdin from pooling: %d\n", numfds);					
				}
			}
			if(fds[1].revents & POLLIN)
			{
				if(accept_connection() < 0)
				{
					printf("Communication problem\n");
					return -1;
				}
				printf("Connection accepted...\n");
				fds[2].fd = rw_fd;
				fds[2].events = POLLIN;
				numfds++;

			}
			if(fds[2].revents & POLLIN)
			{
				char recv_buff[1024];
				unsigned int s = sizeof(recv_buff) - 1;
				memset(recv_buff, 0, sizeof(recv_buff));				
				int recv_len = recv_from_client(recv_buff, s);
				if (recv_len == 0)
				{
					printf("Closing connection....\n");
					fds[2].fd = -1;
					close(rw_fd);
				}
				else if (recv_len < 0)
				{
					return recv_len;
				}
				else
				{
					if (strcmp(recv_buff, "Quit") == 0)
					{
						printf("Terminating program...\n");
						for (int i = 0; i < numfds; i++)
						{
							close(fds[i].fd);
						}
						read_flag = 0;						
					}
					else
					{
						store_and_print(recv_buff, recv_len, &store_buff, DELIMITER);
					}

				}
				
			}
			
		}


	}
	free(store_buff);   

   exit(EXIT_SUCCESS);
}
