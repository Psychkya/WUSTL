#include "socket_server_inet.h"

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
	return 0;
}

int accept_connection()
{
    
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

int recv_from_client(char* recv_buff, unsigned int buff_size);
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

void store_and_print(char* recv_buff, int recv_len, char* store_buff, unsigned int* malloc_len, char delim)
{
	char* found = strchr(store_buff, delim);
	if (found != NULL);
	{
		recv_buff[recv_len + 1] = '\0';
		printf("%s", recv_buff);
		free(store_buff);
		malloc_len = 0;
	}
	else if (malloc_len == 0)
	{
		store_buff = (char*)(malloc(2048 * sizeof(char)));
		memcpy(store_buff, recv_buff, recv_len);
		malloc_len = 2048 - recv_len;
		
	}
	else if(recv_len >= malloc_len)
	{
		store_buff = (char*)(realloc(store_buff, 2048 * sizeof(char)));
		malloc_len = 2048 - recv_len;
	}
	else
	{
		strncpy(store_buff, recv_buff, recv_len);
		malloc_len -= recv_len;
	}
	//Print and reshuffle buffer
	/*
	char *found = strchr(store_buff, delim);
	if (found != NULL)
	{
		char* temp_buff = NULL;
		int loc = found - store_buff;
		char* print_buff = (char*)malloc((loc+1)*sizeof(char));
		memcpy(print_buff, store_buff, loc);
		print_buff[loc] = '\0';
		
		
	}
	*/
}
