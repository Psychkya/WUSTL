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
#define IP_ADDR "128.252.167.161"
//#define IP_ADDR "127.0.0.1"
#define PORT 9000

int main(int argc, char* argv[])
{
	//FILE* wfile;
	char send_buff[100];
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
	/*
	wfile = fdopen(client_fd, "w");
	if(wfile == NULL)
	{
		perror("Could not open client fd");
		return -1;
	}
	*/
	if (argc > 1)
	{
		if(strcmp(argv[1], "Quit") == 0)
		{
			printf("Sending quit command to server...\n");
			//fputs(argv[1], wfile);
			strcpy(send_buff, argv[1]);
			send(client_fd, send_buff, strlen(send_buff), 0);
		}
		else
		{
			printf("Incorrect commmand entered..proceeding with normal operation\n");
			//fputs("First Message to Server\n", wfile);
			strcpy(send_buff, "First Message to Server\n");
			send(client_fd, send_buff, strlen(send_buff), 0);
			//fputs("Second Message to Server\n",wfile);
			strcpy(send_buff, "Second Message to Server\n");
			send(client_fd, send_buff, strlen(send_buff), 0);
		}
	}
	else
	{
		//fputs("First Message to Server\n", wfile);
		//fputs("Second Message to Server\n",wfile);
		strcpy(send_buff, "First Message to Server\n");
		send(client_fd, send_buff, strlen(send_buff), 0);	
		strcpy(send_buff, "Second Message to Server\n");
		send(client_fd, send_buff, strlen(send_buff), 0);			
	}
	//fclose(wfile);
	close(client_fd);
	return 0;

	
	
}

