#include "read_parse_spec_file.h"


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
				strncpy(*buff + ptr_loc, temp_buff, len_spec);
				ptr_loc  += len_spec;
				malloc_len = total_len - len_spec;
			}
			else
			{
				strncpy(*buff + ptr_loc, temp_buff, len_spec);
				ptr_loc += len_spec;
				malloc_len -= len_spec; 
				
			}
		}
	}
	return ptr_loc;
}

int parse_buffer(char** buff, char** p_buff, int buff_len, int p_len, int ptr_loc)
{
   //start searching from the position of last know LF. The first time it will be the beginning of buffer.
   char* found = strchr(*buff + ptr_loc, '\n'); 
   int loc = 0;
   if (found != NULL)
   {
      loc = found - (*buff + ptr_loc) + 1; //first relative position where LF is located
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

