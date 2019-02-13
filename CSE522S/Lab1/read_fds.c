#include "read_fds.h"


int read_fd (int *fd, char** buff, int len)
{
	int len_spec, ptr_loc = 0, total_len = 0, read_flag = 1;
	char temp_buff[len];
	memset(temp_buff, 0, len);
	int malloc_len = 0;
	
	while(read_flag)
	{
		len_spec = 0;
		memset(temp_buff, 0, len);
		len_spec = read(*fd, temp_buff, len);
		//printf("Len:%d, Data: %d, RS1: %s\n", len_spec, len, temp_buff);
		if (len_spec < 0)
		{
			perror("Read");
			return len_spec;
		}
		else if (len_spec == 0)
		{
			close(*fd);
			read_flag = 0;
		}
		else
		{
			if(len_spec >= malloc_len)
			{
				total_len += 2 * len + 1;
				*buff = (char*)realloc(*buff, total_len);
				//printf("D1: %d, buff: %d, total_len: %d, ptr_loc: %d, malloc: %d\n", len_spec, len, total_len, ptr_loc, malloc_len);
				strncpy(*buff + ptr_loc, temp_buff, len_spec);
				ptr_loc  += len_spec;
				//printf("s2: %s\n", *buff);
				malloc_len = total_len - len_spec;
				//printf("D2: %d, buff: %d, total_len: %d, ptr_loc: %d, malloc: %d\n", len_spec, len, total_len, ptr_loc, malloc_len);
			}
			else
			{
				//printf("S1: %d, buff: %d, total_len: %d, ptr_loc: %d, malloc: %d\n", len_spec, len, total_len, ptr_loc, malloc_len);
				strncpy(*buff + ptr_loc, temp_buff, len_spec);
				ptr_loc = +len_spec;
				malloc_len -= len_spec; 
				//printf("S2: %d, buff: %d, total_len: %d, ptr_loc: %d, malloc: %d\n", len_spec, len, total_len, ptr_loc, malloc_len);
				
			}
		}
	}
	return total_len;
}
