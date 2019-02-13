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
#include "read_fds.h"
#define READ_SIZE 50

int parse_buffer(char** buff, char** p_buff, int buff_len, int p_len, int ptr_loc);

int main (int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Incorrect usage\n");
		printf("Usage: server_lab1 <name of spec file> <port to listen>\n"); //check for port?
		return -1;
	}
	
	int read_spec, tot_len = 0, parse_len = 0, ptr_loc = 0;
	char *read_buff = NULL;
   char *p_buffer = NULL;
	read_spec = open(argv[1], O_RDONLY);
	if (read_spec < 0)
	{
		perror("Open");
		return -1;
	}
	
	tot_len = read_fd(&read_spec, &read_buff, READ_SIZE);
	printf("Len: %d, buff-size: %d, data: %s\n", tot_len, strlen(read_buff), read_buff);
   fflush(stdout);
   p_buffer = malloc(100*sizeof(char));
   memset(p_buffer, 0, 100);
   while (parse_len < strlen(read_buff) && parse_len > -1)
   //for (int i = 0; i < 7; i++)
   {
      parse_len = parse_buffer(&read_buff, &p_buffer, tot_len, 100, ptr_loc); //parse_len also provides next relative position of pointer
      ptr_loc += parse_len;
      printf("len: %d, P buffer: %s\n", parse_len, p_buffer);
   }
   //printf("P buffer: %s\n", p_buffer);
   free(read_buff);
   free(p_buffer);
	return 0;
}

int parse_buffer(char** buff, char** p_buff, int buff_len, int p_len, int ptr_loc)
{
   //start searching from the position of last know LF. The first time it will be the beginning of buffer.
   char* found = strchr(*buff + ptr_loc, '\n'); 
   int loc = 0;
   if (found != NULL)
   {
      loc = found - (*buff + ptr_loc) + 1; //first relative position where LF is located
      printf("ptr loc: %d, loc: %d, size of p buff:%d\n", ptr_loc, loc, p_len);
      //printf("buff: %s\n", *buff + ptr_loc);
      if (loc >= p_len) //if number of bytes to relative position is greater than original length of output buffer, increase it
      {
        *p_buff = malloc((p_len + loc)*sizeof(char));
      }
      memcpy(*p_buff, *buff + ptr_loc, loc - 1); //copy from buffer to output buffer
   }
   else
   {
      if (buff_len - ptr_loc > 0)
      {
		  memcpy(*p_buff, *buff+ptr_loc, buff_len - ptr_loc);
	  }
	  else
	  {
		strcpy(*p_buff, "Not found"); //Not finding the last one - need some additional logic
	  }
	  loc = -1;
   }
   return loc;
}
