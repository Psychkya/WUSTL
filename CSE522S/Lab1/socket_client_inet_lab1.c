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

int main(int argc, char* argv[])
{

	char send_buff[1000];
	char read_buff[1000];
	int server_buff_len[1];
	int first_packet = 1;
	char read_ptr = 0;
	int client_fd;

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
			int ret = read(client_fd, (void*)server_buff_len, sizeof(int));
			first_packet = 0;
			if (ret < 0)
			{
				perror("read");
				return -1;
			}
			printf("Buf len: %d\n", server_buff_len[0]);

		}

		int ret = read(client_fd, read_buff+read_ptr, 25);
		read_ptr += ret;
		if (ret < 0)
		{
			perror("read");
			return -1;
		}
		else if(server_buff_len[0] - read_ptr < 25)
		{
			break;
		}
		else
		{
			printf("Bytes read: %d, From server: %s\n", ret, read_buff);
		}
	}
	printf("Final buffer: %s\n", read_buff);
	if (argc > 1)
	{
		strcpy(send_buff, read_buff);
		strcat(send_buff, argv[1]);
	}
	else
	{
		strcpy(send_buff, read_buff);
	}

	
	int ret = write(client_fd, send_buff, sizeof(send_buff));
	if (ret < 0)
	{
		perror("write");
		return -1;
	}

	close(client_fd);
	return 0;


	
}

