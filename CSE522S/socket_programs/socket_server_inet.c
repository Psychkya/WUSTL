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
#define PORT 9000

void rpi_signal_handler(int rpiSig); //function definition
FILE* rfile;
int server_fd, read_fd;

int main(void)
{
	//signal handling variables
	struct sigaction rpi_sigs;
	rpi_sigs.sa_handler = rpi_signal_handler;
	rpi_sigs.sa_flags = SA_RESTART;
	int ret = sigaction(SIGINT, &rpi_sigs, NULL); //registering signal handler
	if (ret < 0)
	{
		perror("could not register sigaction");
		return 1;
	}
	//signal handling - end
	
	int s_flag = 1;
	struct sockaddr_in s_address;
	char read_buff[100];
	
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
	while(s_flag)
	{
		
		//if((read_fd = accept(server_fd, (struct sockaddr*)&s_address, (socklen_t*)&address_len)) < 0)
		if((read_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) < 0)		
		{
			perror("Connection accept error");
			return -1;
		}
		else
		{
			printf("New connection establised:\n");
		}
		rfile = fdopen(read_fd, "r");
		if(rfile == NULL)
		{
			perror("Could not open accept fd");
			return -1;
		}
		while(fgets(read_buff, sizeof(read_buff), (FILE*)rfile)	!= NULL)
		{
			if (strcmp(read_buff,"Quit") == 0)
			{
				printf("Quiting.....\n");
				s_flag = 0;
			}
			else
			{
				printf("%s", read_buff);
			}
		}
		fclose(rfile);
		rfile = NULL;
		close(read_fd);
		read_fd = -1;
		
	}
	
	
}

void rpi_signal_handler(int rpiSig)
{
	if (rfile != NULL)	fclose(rfile);
	if (read_fd != -1) close(read_fd);
	if (raise(SIGTERM) < 0)
	{
		perror("Error handling SIGTERM");
		exit(1);
	}
	return;	
}
