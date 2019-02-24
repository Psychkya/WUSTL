#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h> 
#include <signal.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define NANO_SEC 1E9
#define SOCK_FILE "/tmp/sockfile"
#define BACK_LOG 50

void parent_handler_SIGUSR1(int rpiSig); //parent handler
//void parent_handler_SIGUSR2(int rpiSig); //parent handler
//void child_handler_SIGUSR1(int rpiSig); //child handler
void child_handler_SIGUSR2(int rpiSig); //child handler

int string_to_int(char* a); //convert string to integer
int is_string_digit(char* a); //check if string is a number
char* fifo_file = "/home/pi/WUSTL/CSE522S/ipc_perf/fifotest";

int num_comm = 0;
char* command;

typedef enum {INIT_SIGUSR1_WAIT, INIT_SIGUSR1_RECV, INIT_SIGUSR2_WAIT, INIT_SIGUSR2_RECV} signal_comm;
signal_comm sigParent = INIT_SIGUSR1_WAIT;
signal_comm sigChild = INIT_SIGUSR2_WAIT;


int main(int argc, char* argv[])
{
	pid_t childPid;
    int pipe_fd[2];

	double elapsed_time = 0;
	
	struct timespec tstart, tend; //variables for getting time

	struct sigaction rpi_sigs_pusr1;

    struct sockaddr_un s_address, c1_address;

    struct sockaddr_in sin_address, cin1_address;

    int server_fd, write_fd;
			
	if (argc != 3)
	{
		printf("Incorrect arguments..\nUsage: ipc_perf_main <number of communication> <communication method>\nNumber of communication should be unsigned integer\n");
		return 0;
	} 
	else
	{
		if ( is_string_digit(argv[1]) < 0)
		{
			printf("Number of communication is not an unsigned integer\nUsage: ipc_perf_main <number of communication> <communication method>\n");
			return 0;
		}
		else
		{
			num_comm = string_to_int(argv[1]);
			command = argv[2];
		}
	}


    //Register signals
    rpi_sigs_pusr1.sa_handler = parent_handler_SIGUSR1;
    rpi_sigs_pusr1.sa_flags = SA_RESTART;
    int ret = sigaction(SIGUSR1, &rpi_sigs_pusr1, NULL); //registering signal handler
    if (ret < 0)
    {
        perror("could not register parent SIGUSR1");
        return -1;
    } 

    //check for commands
	if (strcmp(command, "pipe") == 0)
    {
        //Pipe
        if(pipe(pipe_fd) < 0)
        {
            perror("Pipe");
            return -1;
        }
    }
    else if(strcmp(command, "FIFO") == 0)
    {
       	if (mkfifo(fifo_file,0666) < 0)
        {
            perror("Error creating fifo");
            return 1;
        }
    }
    else if (strcmp(command, "lsock") == 0)
    {
        
        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd < 0)
        {
            perror("socket creation failed");
            return -1;
        }
        memset(&s_address, 0, sizeof(s_address)); //clear structure
        s_address.sun_family = AF_UNIX;
        strcpy(s_address.sun_path, SOCK_FILE);
        unlink(SOCK_FILE);
        if((bind(server_fd, (struct sockaddr*)&s_address, sizeof(s_address))) < 0)
        {
            perror("Lsock Bind error");
            return -1;
        }
    }
    else if (strcmp(command, "socket") == 0)
    {
        int option = 1;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //Need this to reuse address
        if (server_fd < 0)
        {
            perror("socket creation failed");
            return -1;
        }
        
        memset(&sin_address, 0, sizeof(sin_address)); //clear structure
        sin_address.sin_family = AF_INET;
        sin_address.sin_addr.s_addr = INADDR_ANY;
        sin_address.sin_port = htons(9000);

        int address_len = sizeof(sin_address);
        
        if((bind(server_fd, (struct sockaddr*)&sin_address, address_len)) < 0)
        {
            perror("Socket Bind error");
            return -1;
        }        
    }
    else
    {
        printf("A valid command is not entered. Valid commands are signals, pipe, lsock or socket\n");
    }

	childPid = fork();
	
	if (childPid < 0)
	{
		perror("Fork failed");
		return 1;
	}
	else if(childPid == 0)
	{
		//Child process
		
		//register signal handler
		struct sigaction rpi_sigs_cusr2;
		rpi_sigs_cusr2.sa_handler = child_handler_SIGUSR2;
		rpi_sigs_cusr2.sa_flags = SA_RESTART;
		int ret = sigaction(SIGUSR2, &rpi_sigs_cusr2, NULL); //registering signal handler	
        if (ret < 0)
        {
            perror("could not register parent SIGUSR2");
            return -1;
        }         		
		//send sigusr1 to parent
		kill(getppid(), SIGUSR1);
		while(sigChild != INIT_SIGUSR2_RECV);
        //Child received back sigusr2
		//Pipe Comm logic here
        if (strcmp(command, "pipe") == 0)
        {
            char read_buff[PIPE_BUF];
            int len = 0;
            close(pipe_fd[1]);
            do
            {
                len = read(pipe_fd[0], read_buff, PIPE_BUF);
                if (len < 0)
                {
                    perror("Pipe read");
                    exit(-1);                    
                }
            }while(len > 0);

        }
        //FIFO comm logic here
        if (strcmp(command, "FIFO") == 0)
        {
            char read_buff[PIPE_BUF];
            int len = 0;
            int fifo_fd = open(fifo_file, O_RDONLY);
            do
            {
                len = read(fifo_fd, read_buff, PIPE_BUF);
                if (len < 0)
                {
                    perror("FIFO read");
                    exit(-1);                    
                }
            }while(len > 0);
            close(fifo_fd);
        }
        //lsock comm logic here
        if (strcmp(command, "lsock") == 0)  
        {
            int client_fd;
            struct sockaddr_un c_address;
            char read_buff[PIPE_BUF];
            int len = 0;            
            client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
            if (client_fd < 0)
            {
                perror("Client socket creation failed");
                exit(-1);
            }
            memset(&c_address, 0, sizeof(c_address)); //clear structure
            c_address.sun_family = AF_UNIX;
            strcpy(c_address.sun_path, SOCK_FILE);            
            if((connect(client_fd, (struct sockaddr*)&c_address, sizeof(c_address))) < 0)
            {
                perror("Client Connect error");
                return -1;
            }
            do
            {
                len = read(client_fd, read_buff, PIPE_BUF);
                if (len < 0)
                {
                    perror("lsock read");
                    exit(-1);                    
                }
            }while(len > 0); 
            close(client_fd);           
        }
        //socket comm logic
        if(strcmp(command,"socket") == 0)
        {
            int client_fd;
            struct sockaddr_in c_address;
            char read_buff[PIPE_BUF];
            int len = 0;             

            client_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (client_fd < 0)
            {
                perror("socket creation failed");
                return -1;
            }
            
            memset(&c_address, 0, sizeof(c_address)); //clear structure
            c_address.sin_family = AF_INET;
            c_address.sin_port = htons(9000);
            inet_aton("127.0.0.1", &c_address.sin_addr);
            if((connect(client_fd, (struct sockaddr*)&c_address, sizeof(c_address))) < 0)
            {
                perror("socket Connect error");
                return -1;
            }   
            do
            {
                len = read(client_fd, read_buff, PIPE_BUF);
                if (len < 0)
                {
                    perror("socket read");
                    exit(-1);                    
                }
            }while(len > 0); 
            close(client_fd);                       
        }
		exit(0);
	}
	else
	{
		//Parent process
		while(sigParent != INIT_SIGUSR1_RECV);
        //Parent got sigusr1 now send signusr2 to complete handshake
		kill(childPid, SIGUSR2);
        //Pipe comm logic
        if (strcmp(command, "pipe") == 0)
        {
            char write_buff[PIPE_BUF];
            memset(write_buff,0,PIPE_BUF);
            close(pipe_fd[0]);
		    clock_gettime(CLOCK_MONOTONIC_RAW,&tstart);            
            for (int i = 0; i < num_comm; i++)
            {
                if (write(pipe_fd[1], write_buff, PIPE_BUF) < 0)
                {
                    perror("Pipe write");
                    return -1;
                }
            }
            close(pipe_fd[1]);
            
        }
        //FIFO comm logic
        if(strcmp(command, "FIFO") == 0)
        {
            char write_buff[PIPE_BUF];
            int fifo_fd = open(fifo_file, O_WRONLY);
            memset(write_buff,0,PIPE_BUF);
		    clock_gettime(CLOCK_MONOTONIC_RAW,&tstart);            
            for (int i = 0; i < num_comm; i++)            
            {
                if(write(fifo_fd, write_buff, PIPE_BUF) < 0)
                {
                    perror("FIFO write");
                    return -1;
                }
            }
        }
        //lsock comm logic
        if(strcmp(command, "lsock") == 0) 
        {
            char write_buff[PIPE_BUF]; 
            memset(write_buff,0,PIPE_BUF);       

            if ((listen(server_fd, 50)) < 0)
            {
                perror("Listen error");
                return -1;
            }
            socklen_t c_len = sizeof(c1_address);	
            if((write_fd = accept(server_fd, (struct sockaddr*)&c1_address, &c_len)) < 0)
            {
                perror("Connection accept error");
                return -1;
            }
            else
            {
                clock_gettime(CLOCK_MONOTONIC_RAW,&tstart);
                for (int i = 0; i < num_comm; i++)            
                {
                    if(write(write_fd, write_buff, PIPE_BUF) < 0)
                    {
                        perror("lsock write");
                        return -1;
                    }
                }               
            }            
        }
        //socket comm logic
        if(strcmp(command, "socket") == 0)
        {
            char write_buff[PIPE_BUF]; 
            memset(write_buff,0,PIPE_BUF);            
            if ((listen(server_fd, 50)) < 0)
            {
                perror("Listen error");
                return -1;
            }
            socklen_t c_len = sizeof(cin1_address);	
            if((write_fd = accept(server_fd, (struct sockaddr*)&cin1_address, &c_len)) < 0)		
            {
                perror("Connection accept error");
                return -1;
            }
            else
            {
                clock_gettime(CLOCK_MONOTONIC_RAW,&tstart);
                for (int i = 0; i < num_comm; i++)            
                {
                    if(write(write_fd, write_buff, PIPE_BUF) < 0)
                    {
                        perror("Socket write");
                        return -1;
                    }
                } 
            }  
        }
        close(write_fd);
        close(server_fd);
		clock_gettime(CLOCK_MONOTONIC_RAW,&tend);
		elapsed_time = (tend.tv_sec - tstart.tv_sec) * NANO_SEC + (tend.tv_nsec - tstart.tv_nsec);	
		printf("%s Elapsed Time:%f\n", command, elapsed_time);

	}
	unlink(fifo_file);
	return 0;
}

void parent_handler_SIGUSR1(int rpiSig)
{
	sigParent  = INIT_SIGUSR1_RECV;
}


void parent_handler_SIGUSR2(int rpiSig)
{
	sigParent = INIT_SIGUSR2_RECV;
}

void child_handler_SIGUSR2(int rpiSig)
{
	sigChild = INIT_SIGUSR2_RECV;
}

int is_string_digit(char* a)
{
	int result = -1;
	int len = strlen(a);
	for (int i = 0; i < len; i++)
	{
		if(a[i] >= '0' && a[i] <= '9')
		{
			result = 0;
		}
		else
		{
			result = -1;
			break;
		}
	}
	return result;

}

int string_to_int(char* a)
{
	int result = 0;
	int len = strlen(a);
	for (int i = 0; i < len; i++)
	{
		result = result * 10 + (a[i] - '0');
	}
	return result;
}


