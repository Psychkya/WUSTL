#include "socket_server_inet_lab1.h"

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
	s_address.sin_port = htons(server_port);


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

void store_and_print(char* recv_buff, int recv_len, char* store_buff, unsigned int* malloc_len, char delim) //check - should be strcat instead of strncpy (or use ptr math)
{
	char* found = strchr(recv_buff, delim);
	if (found != NULL)
	{
		if (store_buff == NULL)
		{
			recv_buff[recv_len + 1] = '\0';
		}
		else
		{
			if (recv_len >= *malloc_len)
			{
				store_buff = (char*)(realloc(store_buff, (*malloc_len + recv_len + 1) * sizeof(char)));
			}
			strncpy(store_buff, recv_buff, recv_len);
			int store_len = strlen(store_buff);
			store_buff[store_len + 1] = '\0';
			free(store_buff);
			store_buff = NULL;
			*malloc_len = 0;
		}
	}
	else if (*malloc_len == 0)
	{
		store_buff = (char*)(malloc(2048 * sizeof(char)));
		memcpy(store_buff, recv_buff, recv_len);
		*malloc_len = 2048 - recv_len;
		
	}
	else if(recv_len >= *malloc_len)
	{
		store_buff = (char*)(realloc(store_buff, 2048 * sizeof(char)));
		strncpy(store_buff, recv_buff, recv_len);
		*malloc_len = 2048 - recv_len;
	}
	else
	{
		strncpy(store_buff, recv_buff, recv_len);
		*malloc_len -= recv_len;
	}
}
