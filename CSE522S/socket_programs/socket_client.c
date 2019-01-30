#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SOCK_FILE "/tmp/sockfile"
#define BACK_LOG 50

int main(int argc, char* argv[])
{
	FILE* wfile;
	int client_fd;

	struct sockaddr_un c_address;
	//char read_buff[100];
	
	client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		perror("socket creation failed");
		return -1;
	}
	
	memset(&c_address, 0, sizeof(c_address)); //clear structure
	c_address.sun_family = AF_UNIX;
	strcpy(c_address.sun_path, SOCK_FILE);
	
	if((connect(client_fd, (struct sockaddr*)&c_address, sizeof(c_address))) < 0)
	{
		perror("Connect error");
		return -1;
	}
	wfile = fdopen(client_fd, "w");
	if(wfile == NULL)
	{
		perror("Could not open client fd");
		return -1;
	}
	if (argc > 1)
	{
		if(strcmp(argv[1], "Quit") == 0)
		{
			printf("Sending quit command to server...\n");
			fputs(argv[1], wfile);
		}
		else
		{
			printf("Incorrect commmand entered..proceeding with normal operation\n");
			fputs("First Message to Server\n", wfile);
			fputs("Second Message to Server\n",wfile);
		}
	}
	else
	{
		fputs("First Message to Server\n", wfile);
		fputs("Second Message to Server\n",wfile);
	}
	fclose(wfile);
	close(client_fd);
	return 0;

	
	
}

