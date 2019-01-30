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

int main(void)
{
	FILE* rfile;
	int server_fd, read_fd;
	
	struct sockaddr_un s_address;
	char read_buff[100];
	
	server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		perror("socket creation failed");
		return -1;
	}
	
	memset(&s_address, 0, sizeof(s_address)); //clear structure
	s_address.sun_family = AF_UNIX;
	strcpy(s_address.sun_path, SOCK_FILE);
	
	if((bind(server_fd, (struct sockaddr*)&s_address, sizeof(s_address))) < 0)
	{
		perror("Bind error");
		return -1;
	}
	if ((listen(server_fd, 50)) < 0)
	{
		perror("Listen error");
		return -1;
	}
	if((read_fd = accept(server_fd, NULL, NULL)) < 0)
	{
		perror("Connection accpet error");
		return -1;
	}
	rfile = fdopen(read_fd, "r");
	if(rfile == NULL)
	{
		perror("Could not open accept fd");
		return -1;
	}
	while(fgets(read_buff, sizeof(read_buff), (FILE*)rfile)	!= NULL)
	{
		printf("%s", read_buff);
	}
	fclose(rfile);
	close(read_fd);
	unlink(SOCK_FILE); //unlink the file before exiting
	
}

