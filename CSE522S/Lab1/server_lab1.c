#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include "read_parse_spec_file.h"
#include "socket_server_inet_lab1.h"
#define READ_SIZE 50
#define NUM_FRAG_FILES 100
//#define NUM_POLLS 1000 //server socket + n number of client fds to poll for

int main (int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Incorrect usage\n");
		printf("Usage: server_lab1 <name of spec file> <port to listen>\n"); //check for port?
		return -1;
	}
	//define file descriptors
  int spec_file_fd;
  int *frag_file_fd = malloc(NUM_FRAG_FILES * sizeof(int)); //fds of fragmented files. first fd is the final file
  struct pollfd *poll_fds = NULL;

	int tot_len = 0, parse_len = 0, ptr_loc = 0, frag_file_count = 0, malloc_frag_fd = NUM_FRAG_FILES, num_poll_fds = 1, retval = 0;
	char *read_buff = NULL; //buffer to read entire spec file
  char *p_buffer = NULL; //buffer to retrieve name of each fragmented file
	spec_file_fd = open(argv[1], O_RDONLY);
	if (spec_file_fd < 0)
	{
		perror("Open");
		return -1;
	}
	
	tot_len = read_fd(&spec_file_fd, &read_buff, READ_SIZE);
	printf("Len: %d, buff-size: %d, data: %s\n", tot_len, strlen(read_buff), read_buff);
  fflush(stdout);
  p_buffer = malloc(100*sizeof(char));
  memset(p_buffer, 0, 100);
  while (parse_len < strlen(read_buff) && parse_len > -1)
  {
    parse_len = parse_buffer(&read_buff, &p_buffer, tot_len, 100, ptr_loc); //parse_len also provides next relative position of pointer
    ptr_loc += parse_len;
    printf("len: %d, P buffer: %s\n", parse_len, p_buffer);
    if (frag_file_count == 0)
    {
      if ((frag_file_fd[frag_file_count++] = open(p_buffer, O_WRONLY | O_CREAT)) < 0) 
      {
        perror("Final file open:");
        return -1;
      }
    }
    else
    {
      if (frag_file_count >= malloc_frag_fd)
      {
        malloc_frag_fd += frag_file_count + NUM_FRAG_FILES;
        frag_file_fd = (int*)realloc(frag_file_fd, malloc_frag_fd * sizeof(int));
      }
      if ((frag_file_fd[frag_file_count++] = open(p_buffer, O_RDONLY)) < 0)
      {
        perror("Frag file open");
        return -1;
      }
    }
    
  }
  printf("Num frag files: %d\n", frag_file_count);
  for (int i = 0; i < frag_file_count; i++) printf("Frag fd: %d\n", frag_file_fd[i]);

  poll_fds = (struct pollfd*)malloc(frag_file_count * sizeof(struct pollfd));

  //Creating server socket
  if(create_socket() < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}
  poll_fds[0].fd = server_fd; //add server socket to be poll list
  poll_fds[0].events = POLLIN;

  //TESTING CODE
  char *write_to_client = "Client! do you copy?! Calling client one two three\n";
  int check_len = strlen(write_to_client);
  char server_r_buffer[1000];
  //TESTING CODE END


  int send_len_to_client[1];
  int relative_write_ptr = 0;
  int first_packet = 1;
  int relative_read_ptr = 0;


  while(1)
  {
    retval = poll(poll_fds, num_poll_fds, -1);
    if (retval < 0)
    {
      /*handle error*/
    }
    for(int i = 0; i < num_poll_fds; i++)
    {
      if(poll_fds[i].revents & POLLIN)
			{
        if (poll_fds[i].fd == server_fd)
        {
          if(accept_connection() < 0)
          {
            printf("Communication problem\n");
            return -1;
          }
          printf("Connection accepted...\n");
          poll_fds[i + num_poll_fds].fd = rw_fd;
          poll_fds[i + num_poll_fds].events = POLLOUT;
          num_poll_fds++;
        }
        else
        {
          /* read stuff */
          int len = read(poll_fds[i].fd, server_r_buffer + relative_read_ptr, 25);
          relative_read_ptr += len;
          printf("From client: %s\n", server_r_buffer);
          if (len == 0) poll_fds[i].fd = -1;
        }
        
			}
      if (poll_fds[i].revents & POLLOUT)
      {
        /*write stuff*/
        /* if write returns 0, set the event value of fd to POLLIN */
        if (first_packet)
        {
          send_len_to_client[0] = check_len;
          int len = write(poll_fds[i].fd, (void*)send_len_to_client, sizeof(int));
          first_packet = 0;
        }
        if (check_len > 0 && check_len < 10)
        {
          printf ("sending to client1: %s\n", write_to_client + relative_write_ptr);
          int len = write(poll_fds[i].fd, write_to_client + relative_write_ptr, 10 - check_len);
          poll_fds[i].events = POLLIN;
          check_len -= len;
          printf("check_len1: %d, len: %d\n", check_len, len);
         }
        else if (check_len <= 0)
        {
          poll_fds[i].events = POLLIN;            
        }
        else
        {
          printf ("sending to client: %s\n", write_to_client + relative_write_ptr);
          int len = write(poll_fds[i].fd, write_to_client + relative_write_ptr, 10);
          check_len -= len;
          relative_write_ptr += len;
          printf("check_len3: %d, len: %d\n", check_len, len);

        }
      }
    }
  }

  free(read_buff);
  free(p_buffer);
  free(frag_file_fd);
  free(poll_fds);
	return 0;
}
