#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define READ_SIZE 50

int read_fd(int*  fd, char** buff, int len);

int main (int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Incorrect usage\n");
		printf("Usage: server_lab1 <name of spec file> <port to listen>\n"); //check for port?
		return -1;
	}
	
	int read_spec, tot_len = 0;
	char *read_buff = NULL;
	read_spec = open(argv[1], O_RDONLY);
	if (read_spec < 0)
	{
		perror("Open");
		return -1;
	}
	
	tot_len = read_fd(&read_spec, &read_buff, READ_SIZE);
	printf("Len: %d, buff-size: %d, data: %s\n", tot_len, sizeof(read_buff), read_buff);
			
	return 0;
}

int read_fd (int *fd, char** buff, int len)
{
	int len_spec, total_len = 0, read_flag = 1;
	char temp_buff[len];
	memset(temp_buff, 0, sizeof(temp_buff));
	int malloc_len = 0;
	
	while(read_flag)
	{
		len_spec = 0;
		memset(temp_buff, 0, sizeof(temp_buff));
		len_spec = read(*fd, temp_buff, sizeof(temp_buff));
		printf("Len:%d, Data: %d, RS1: %s\n", len_spec, sizeof(temp_buff), temp_buff);
		if (len_spec < 0)
		{
			perror("Read");
			return len_spec;
		}
		else if (len_spec == 0)
		{
			close(*fd);
			read_flag = 0;
		}
		else
		{
			malloc_len += len_spec;
			if(malloc_len >= len)
			{
				*buff = (char*)realloc(*buff, len);
				printf("D1: %d, buff: %d, s1: %s\n", len_spec, sizeof(temp_buff), temp_buff);
				strncpy(*buff + total_len, temp_buff, len_spec);
				total_len  += len_spec;
				printf("s2: %s\n", *buff);
				malloc_len = 0;
			}
			else
			{
				printf("ss1: %s\n", temp_buff);
				strncpy(*buff + total_len, temp_buff, len_spec);
				total_len = +len_spec;
				printf("ss2: %s\n", *buff);
				
			}
		}
	}
	return total_len;
	
}
