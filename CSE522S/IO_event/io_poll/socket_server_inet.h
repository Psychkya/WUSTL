#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#define BACK_LOG 50
#define PORT 9000

int server_fd, rw_fd;
struct sockaddr_in s_address, c_address;

int create_socket();
int accept_connection();
int send_to_client(char* send_buff, unsigned int size);
int recv_from_client(char* recv_buff, unsigned int size);
void store_and_print(char* recv_buff, int recv_len, char* store_buff, unsigned int* malloc_len, char delim);


