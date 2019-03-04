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

int talk_2_client()
{
    char host_name[100];
	if (gethostname(host_name, 100) < 0)
	{
		strcpy(host_name, "hostname error\n");
	}
	char write_buff[2048];
	strcpy(write_buff, host_name);
	strcat(write_buff, "\n");
	time_t curr_time = time(NULL);
	char* time_string = ctime(&curr_time);
	strcat(write_buff, time_string);


    
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

	send(rw_fd, write_buff, strlen(write_buff), 0);
	close(rw_fd);
	return 0;
		
}
