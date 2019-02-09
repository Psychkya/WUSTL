/* USING POLL*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
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

	if(create_socket() < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[1].fd = server_fd;
	fds[1].fd = POLLIN;
	
	while(read_flag)
	{
		
		retval = poll (fds, 2, -1); //poll indefinitely

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
					printf("Len: %d, Data: %s\n", len, buff);

				}
				else if (ret < 0)
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
			if(fds[1].revents & POLLIN)
			{
				if(talk_2_client() < 0)
				{
					printf("Communication problem\n");
					return -1;
				}
				fds[2].fd = rw_fd;
				fds[2].events = POLLIN;

			}
			if(fds[2].revents & POLLIN)
			{
				char recv_buff[1024];
				unsigned int s = sizeof(recv_buff) - 1;
				
				int recv_len = recv_from_client(recv_buff, s);
				if (recv_len == 0)
				{
					fds[1].fd = -1;
					fds[2].fd = -1;
					close(server_fd);
					close(rw_fd);
				}
				else
				{
					store_and_print(recv_buff, recv_len, store_buff, &malloc_len, DELIMITER);

				}
				
			}
		}


	}
	   

   exit(EXIT_SUCCESS);
}
