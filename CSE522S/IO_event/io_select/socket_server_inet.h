#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define BACK_LOG 50
#define PORT 9000

int server_fd, rw_fd;
struct sockaddr_in s_address, c_address;

int create_socket();
int talk_2_client();



