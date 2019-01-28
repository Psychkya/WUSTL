/*
 * Reference: 1) Linux Programmer's Guide - chapter 6 http://tldp.org/LDP/lpg/lpg.html
 *            2) www.gnu.org/software/libc/manual/html_node/Creating-a-Pipe.html
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

void write_pipe(int file_d);
void read_pipe(int file_d);

int main(void)
{
	int fd[2];
	pid_t childPid;
	
	int pipe_error = pipe(fd);
	
	if (pipe_error < 0)
	{
		perror("Pipe failed");
		return 1;
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
		close(fd[0]);
		//char* msg = "Hello Mama!\n";
		//write(fd[1], msg, (strlen(msg)+1)); //writing without file stream
		
		write_pipe(fd[1]); //using file stream
		
		exit(0);
	}
	else
	{
		//Parent process
		close(fd[1]);
		//char readbuff[50];
		//int len = read(fd[0], readbuff, sizeof(readbuff));
		//printf("Received length: %d; Received string: %s", len, readbuff); //reading straight from file descriptor
		
		read_pipe(fd[0]); //using file stream
	}
	
	return 0;
}
void write_pipe(int file_d)
{
	char* msg1 = "Hello Mama!\n";
	char* msg2 = "Acknowledge your child!\n";
	FILE* wfile;
	wfile = fdopen(file_d, "w");
	fputs(msg1, wfile);
	fputs(msg2, wfile);
	fclose(wfile);
}

void read_pipe(int file_d)
{
	FILE* rfile;
	rfile = fdopen(file_d, "r");
	char read_buff[100];
	//int c;
	//while((c = fgetc(rfile)) != EOF)
	//{
	//	putchar(c);
	//}
	while(fgets(read_buff, sizeof(read_buff), (FILE*)rfile)	!= NULL)
	{
		printf("%s", read_buff);
	}	
	fclose(rfile);		
}
