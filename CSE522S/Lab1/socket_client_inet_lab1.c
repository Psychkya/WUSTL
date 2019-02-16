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

#define SOCK_FILE "/tmp/sockfile"
#define BACK_LOG 50
//#define IP_ADDR "128.252.167.161"
#define IP_ADDR "127.0.0.1"
#define PORT 9000
#define READ_SIZE 1456

int main(int argc, char* argv[])
{

	char *send_buff;
	char *read_buff = malloc(READ_SIZE * sizeof(char));
	int temp_buff_len;
	int server_buff_len;
	int first_packet = 1;
	int read_ptr = 0;
	int write_ptr = 0;
	int client_fd;
	int ret;

	struct sockaddr_in c_address;
		
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		perror("socket creation failed");
		return -1;
	}
	
	memset(&c_address, 0, sizeof(c_address)); //clear structure
	c_address.sin_family = AF_INET;
	c_address.sin_port = htons(PORT);
	inet_aton(IP_ADDR, &c_address.sin_addr);
	
	if((connect(client_fd, (struct sockaddr*)&c_address, sizeof(c_address))) < 0)
	{
		perror("Connect error");
		return -1;
	}

	while (1)
	{
		if (first_packet)
		{
			int ret = read(client_fd, &temp_buff_len, sizeof(server_buff_len));
			if (ret < 0)
			{
				perror("read");
				return -1;
			}
			server_buff_len = ntohl(temp_buff_len);
			first_packet = 0;
			printf("Buf len: %d\n", server_buff_len);

		}

		if ((server_buff_len - read_ptr) > 0 && (server_buff_len - read_ptr) < READ_SIZE)
		{
			ret = read(client_fd, read_buff+read_ptr, (server_buff_len - read_ptr));
			if (ret < 0)
			{
				perror("read");
				return -1;
			}
			break;
		}	
		else if ((server_buff_len - read_ptr) <= 0)
		{
			break;
		}
		else
		{
			ret = read(client_fd, read_buff+read_ptr, READ_SIZE);
			if (ret < 0)
			{
				perror("read");
				return -1;
			}			
			read_ptr += ret;
			if (server_buff_len - read_ptr >= READ_SIZE)
			{
				read_buff = realloc(read_buff, READ_SIZE + (server_buff_len - read_ptr) + 1);
			}
		}
		
	}
	printf("Final buffer: %s\n", read_buff);

	first_packet = 1;
	send_buff = malloc(server_buff_len * sizeof(char));

	memcpy(send_buff, read_buff, server_buff_len);

	while(1)
	{
		if (first_packet)
		{
          int send_len_to_server = htonl(server_buff_len);
          int len = write(client_fd, &send_len_to_server, sizeof(send_len_to_server));
          if (len < 0)
          {
            perror("write to server");
            return -1;
          }   
          first_packet = 0;			
		}
		if (server_buff_len > 0 && server_buff_len < READ_SIZE)
		{
			int len = write(client_fd, send_buff + write_ptr, READ_SIZE - server_buff_len);
			if (len < 0)
			{
				perror("write to server");
				return -1;
			}   			
			server_buff_len -= len;
			break;
		}
		else if (server_buff_len <= 0)
		{
			break;
		}
		else
		{
			int len = write(client_fd, send_buff + write_ptr, READ_SIZE);
			if (len < 0)
			{
				perror("write to server");
				return -1;
			}   			
			server_buff_len -= len;
			write_ptr += len;
		}
	}


	free(read_buff);
	free(send_buff);
	close(client_fd);
	return 0;


	
}

