#include "socket_server_inet.h"
#define BUFF_SIZE 2048

int ptr_loc = 0;
int buff_len = 0;
int malloc_len = 0;

int create_socket()
{

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		perror("socket creation failed");
		return -1;
	}
	
	memset(&s_address, 0, sizeof(s_address)); //clear structure
	s_address.sin_family = AF_INET;
	s_address.sin_addr.s_addr = INADDR_ANY;
	s_address.sin_port = htons(PORT);


	int address_len = sizeof(s_address);
	
	if((bind(server_fd, (struct sockaddr*)&s_address, address_len)) < 0)
	{
		perror("Bind error");
		return -1;
	}
	if ((listen(server_fd, 50)) < 0)
	{
		perror("Listen error");
		return -1;
	}
	printf("Server is listening...\n");
	return 0;
}

int accept_connection()
{
	printf("Accepting connection....\n");
    
    int c_len = sizeof(c_address);		
	if((rw_fd = accept(server_fd, (struct sockaddr*)&c_address, &c_len)) < 0)		
	{
		perror("Connection accept error");
		return -1;
	}
	else
	{
		printf("New connection establised\n");
		fflush(stdout);
	}
	return 0;
}

int send_to_client(char* send_buff, unsigned int buff_size)
{
	send(rw_fd, send_buff, buff_size, 0);
	close(rw_fd);
	return 0;

}

int recv_from_client(char* recv_buff, unsigned int buff_size)
{

	int ret = read(rw_fd, recv_buff, buff_size);
	if (ret < 0)
	{
		perror("Server read");
		return ret;
	}
	else if (ret == 0)
	{
		return ret;
	}
	else
	{
		return strlen(recv_buff);
	}
		
}

void store_and_print(char* recv_buff, int recv_len, char** store_buff, char delim) //check - should be strcat instead of strncpy (or use ptr math)
{
	if (recv_len >= malloc_len - ptr_loc)
	{
		malloc_len += (recv_len + BUFF_SIZE);
		*store_buff = (char*)realloc(*store_buff, malloc_len*sizeof(char));
	}
	strncpy(*store_buff + buff_len, recv_buff, recv_len);
	int found_flag = 1;
	while( found_flag )
	{
		char* found = strchr(*store_buff + ptr_loc, delim);
		if (found != NULL)
		{
			int loc = found - (*store_buff + ptr_loc) + 1;
			char *print_buf = malloc(loc+1 *sizeof(char));
			strncpy(print_buf, *store_buff + ptr_loc, loc);
			print_buf[loc+1] = '\0';
			printf("%s", print_buf);
			free(print_buf);
			ptr_loc += loc;
			if (ptr_loc >= recv_len)
			{
				found_flag = 0;
				ptr_loc = 0;
				buff_len = 0;
				memset(*store_buff, 0, malloc_len);
			}
		}
		else
		{
			found_flag = 0;
			buff_len += recv_len;
		}
	}
}
