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
#include "sort_buffer_AVL.h"
#define READ_SIZE 1456   //length that can be sent over a socket without defragmentation
#define WRITE_SIZE 1456  //length that can be sent over a socket without defragmentation
#define NUM_FRAG_FILES 100  //Initial number of frag files
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

	int tot_len = 0, parse_len = 0, ptr_loc = 0, frag_file_count = 0, malloc_frag_fd = NUM_FRAG_FILES, num_poll_fds = 1, num_track_fds = 1, retval = 0;
	char *read_buff = NULL; //buffer to read entire spec file
  char *p_buffer = NULL; //buffer to retrieve name of each fragmented file

	spec_file_fd = open(argv[1], O_RDONLY); //open spec file
	if (spec_file_fd < 0)
	{
		perror("Open");
		return -1;
	}
	
	tot_len = read_fd(&spec_file_fd, &read_buff, READ_SIZE); //read entire contents of spec file to read_buff
  p_buffer = malloc(100*sizeof(char)); //allocate a size to buffer that will hold each fragment file name

  //parse through read_buff and get each file name. Keep a count of files in frag_file_count
  while (parse_len < strlen(read_buff) && parse_len > -1)
  {

    parse_len = parse_buffer(read_buff, &p_buffer, tot_len, ptr_loc); //ptr_loc provides next relative position of pointer
    ptr_loc += parse_len;
    //printf("P buffer: %s", p_buffer);
    //Remove the newline character to get each file name
    char *file_name = malloc(strlen(p_buffer) * sizeof(char));
    strncpy(file_name, p_buffer, strlen(p_buffer) - 1); 
    //printf("File name: %s", file_name);
    if (frag_file_count == 0) //the first line is the name of the final file
    {
      if ((frag_file_fd[frag_file_count++] = open(file_name, O_WRONLY | O_CREAT)) < 0) //open the first file as write-create
      {
        perror("Final file open:");
        return -1;
      }
    }
    else
    {
      if (frag_file_count >= malloc_frag_fd) //the second file onwards open each file as read only
      {
        malloc_frag_fd += frag_file_count + NUM_FRAG_FILES;
        frag_file_fd = (int*)realloc(frag_file_fd, malloc_frag_fd * sizeof(int));
      }
      if ((frag_file_fd[frag_file_count++] = open(file_name, O_RDONLY)) < 0)
      {
        perror("Frag file open");
        return -1;
      }
    }
    free(file_name);
    if (tot_len - ptr_loc <= 0) break;
  }

  //we do not need read_buff and p_buffer anymore. Free these
  free(read_buff);
  free(p_buffer);

  //create an array of buffers to hold contents to be sent to each client. The size of this array will be equal to number of file fragments - 1;
  char **client_send_buffer;
  client_send_buffer = malloc((frag_file_count - 1) * sizeof(char*));
  int client_send_len[frag_file_count - 1]; //hold the length of each client send buffer
  
  
  //create an array of buffers to hold contents received from each client
  char **server_recv_buffer;
  server_recv_buffer = malloc((frag_file_count - 1) * sizeof(char *));
  int server_recv_len[frag_file_count - 1]; //hold length of each buffer received from server

  //House keeping variables
  int relative_write_ptr[frag_file_count - 1]; //keep track of write pointer for each client
  int relative_read_ptr[frag_file_count - 1]; //Keep track of read pointer for each client  
  int first_packet[frag_file_count - 1]; //The first packet is always the length. Keep track of it


  //Initialize the int arrays
  for (int i = 0; i < frag_file_count - 1; i++)
  {
    relative_read_ptr[i] = 0;
    relative_write_ptr[i] = 0;
    first_packet[i]  = 1;
    client_send_len[i] = 0;
    server_recv_len[i]  = 0;
  }

  //read each fragmented file into its respective buffer
  for (int i = 0; i < frag_file_count - 1; i++)
  {
    client_send_buffer[i] = NULL; //initialize each buffer to NULL
    client_send_len[i] = read_fd(&frag_file_fd[i+1], &client_send_buffer[i], READ_SIZE); //note that first file fd is final final. hence, fd starts at i+1
  }

  //allocate a size equal to number of files to poll_fds
  poll_fds = (struct pollfd*)malloc(frag_file_count * sizeof(struct pollfd));

  //Create server socket
  if(create_socket() < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}
  poll_fds[0].fd = server_fd; //add server socket to be poll list
  poll_fds[0].events = POLLIN;

  while(1)
  {
    if (num_track_fds <= 0)
    {
      printf("All procesing complete\n");
      break;
    }    
    retval = poll(poll_fds, num_poll_fds, -1);
    if (retval < 0)
    {
      perror("poll");
      return -1;
    }
    for(int i = 0; i < num_poll_fds; i++)
    {
      if(poll_fds[i].revents & POLLIN)
			{
        if (poll_fds[i].fd == server_fd && num_poll_fds <= frag_file_count) //if it is server fd AND number of connections made is less than number of frag file
        {
          if(accept_connection() < 0)
          {
            printf("Communication problem\n");
            return -1;
          }
          poll_fds[i + num_poll_fds].fd = rw_fd;
          poll_fds[i + num_poll_fds].events = POLLOUT;
          first_packet[i] = 1;
          num_poll_fds++;            
          num_track_fds++; //this will be decreased when an fd is no longer polled
          if (num_poll_fds >= frag_file_count)
          {
            poll_fds[i].fd = -1;              
            num_track_fds--;
          }
                    
        }
        else
        {
          /* read stuff */
          int temp_buff_len = 0;
          if (first_packet[i-1])
          {
            int ret = read(poll_fds[i].fd, &temp_buff_len, sizeof(temp_buff_len));
            if (ret < 0)
            {
              perror("read");
              return -1;
            }
            server_recv_len[i-1] = ntohl(temp_buff_len);
            first_packet[i-1] = 0;
            server_recv_buffer[i-1] = malloc((server_recv_len[i-1] + 1) * sizeof(char)); //since we know length - malloc it
          }
          if ((server_recv_len[i-1] - relative_read_ptr[i-1]) > 0 && (server_recv_len[i-1] - relative_read_ptr[i-1]) <= READ_SIZE)
          {
            int ret = read(poll_fds[i].fd, server_recv_buffer[i-1] + relative_read_ptr[i-1], (server_recv_len[i-1] - relative_read_ptr[i-1]));
            if (ret < 0)
            {
              perror("read1");
              return -1;
            }
            poll_fds[i].fd = -1;
            num_track_fds--;
            printf("Buffer from client: %s\n", server_recv_buffer[i-1]);
          }	
          else if ((server_recv_len[i-1] - relative_read_ptr[i-1]) <= 0)
          {
            poll_fds[i].fd = -1;
            num_track_fds--;
          }
          else
          {
            int ret = read(poll_fds[i].fd, server_recv_buffer[i-1]+relative_read_ptr[i-1], READ_SIZE);
            if (ret < 0)
            {
              perror("read2");
              return -1;
            }			
            relative_read_ptr[i-1] += ret;
          }         
        }
        
			}
      if (poll_fds[i].revents & POLLOUT)
      {
        /*write stuff*/
        /* if write returns 0, set the event value of fd to POLLIN */
        if (first_packet[i-1])
        {
          int send_len_to_client = htonl(client_send_len[i - 1]);
          int len = write(poll_fds[i].fd, &send_len_to_client, sizeof(send_len_to_client));
          if (len < 0)
          {
            perror("write to client");
            return -1;
          }          
          first_packet[i-1] = 0;
        }
        if (client_send_len[i-1] > 0 && client_send_len[i-1] <= WRITE_SIZE)
        {
          //int len = write(poll_fds[i].fd, client_send_buffer[i-1] + relative_write_ptr[i-1], WRITE_SIZE - client_send_len[i-1]);
          int len = write(poll_fds[i].fd, client_send_buffer[i-1] + relative_write_ptr[i-1], client_send_len[i-1]);
          if (len < 0)
          {
            perror("write to client");
            return -1;
          }                
          poll_fds[i].events = POLLIN;
          first_packet[i-1] = 1;
          client_send_len[i-1] -= len;
        }
        else if (client_send_len[i-1] <= 0)
        {
          poll_fds[i].events = POLLIN;            
          first_packet[i-1] = 1;
        }
        else
        {
          int len = write(poll_fds[i].fd, client_send_buffer[i-1] + relative_write_ptr[i-1], WRITE_SIZE);
          if (len < 0)
          {
            perror("write to client");
            return -1;
          }                
          client_send_len[i-1] -= len;
          relative_write_ptr[i-1] += len;

        }
      }
    }
  }

  //MERGE LOGIC HERE
  //Reconstruct the structure arrays on the server side

  
  struct Line **temp_avl;
  struct Line **merged_avl;
  int temp_count = 0;
  int first_pass = 1;

  for (int i = 0; i < frag_file_count - 1; i++)
  {
    struct Node *root = NULL; 
    parse_len = 0, ptr_loc = 0;
    int count=0;
    struct Line **server_avl;
    server_avl = malloc(10 * sizeof(struct Line*)); //starting with 10 lines per file
    char* buff_line = malloc(100*sizeof(char));
    while (parse_len < server_recv_len[i] && parse_len > -1)
    {
        if (count >= 10)
        {
            server_avl= realloc(server_avl, (count + 10 * 2) * sizeof(struct Line)); 
        }
        parse_len = parse_buffer(server_recv_buffer[i], &buff_line, server_recv_len[i], ptr_loc); //ptr_loc provides next relative position of pointer
        ptr_loc += parse_len;
        int n = -1;
        server_avl[count]= (struct Line*)malloc(sizeof(struct Line)); 
        int read = sscanf(buff_line, "%d", &n);
        server_avl[count]->content = malloc((strlen(buff_line)+1) * sizeof(char));
        strncpy(server_avl[count]->content, buff_line, strlen(buff_line));
        server_avl[count]->content[strlen(buff_line) + 1] = '\0'; 
        printf("String: %d: %s", strlen(server_avl[count]->content), server_avl[count]->content);
        server_avl[count]->num = n;
        root = insert(root, server_avl[count]); 
        count++;
        if (server_recv_len[i] - ptr_loc <= 0) break;
    }    
    if (!first_pass)
    {
      printf("Seg fault 1\n");
      free(merged_avl);
    }
    first_pass = 0;
    printf("Count: %d, temp_count: %d\n", count, temp_count);
    printf("Seg fault 9\n");
    merged_avl = malloc((temp_count + count) * sizeof(struct Line*));
    printf("Seg fault 10\n");
    merge(server_avl, temp_avl, merged_avl, count, temp_count);
    temp_count += count;
    printf("Seg fault 11\n");
    temp_avl = malloc(temp_count * sizeof(struct Line*));
    printf("Seg fault 12\n");
    for (int i = 0; i < temp_count; i++)
    {
      temp_avl[i] = merged_avl[i];
    }
    printf("Seg fault 2\n");
    free(buff_line);
    printf("Seg fault 3\n");
    printf("Merged AVL here: %d\n",temp_count);
    for (int j=0; j < temp_count; j++)
    {
      printf("%s", merged_avl[j]->content);
    }    
  }  

  printf("Final merge:\n");
  for (int j=0; j < temp_count; j++)
  {
    printf("%s", merged_avl[j]->content);
  }


  for (int i = 0; i < frag_file_count - 1; i++)
  {
    free(server_recv_buffer[i]);
  }
  free(server_recv_buffer);

  free(frag_file_fd);
  free(poll_fds);
  for(int i = 0; i < frag_file_count - 1; i++)
  {
    free(client_send_buffer[i]);
  }
  free(client_send_buffer);

	return 0;
}
