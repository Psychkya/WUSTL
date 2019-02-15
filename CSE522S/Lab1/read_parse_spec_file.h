#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int read_fd(int*  fd, char** buff, int len);
int parse_buffer(char** buff, char** p_buff, int buff_len, int p_len, int ptr_loc);