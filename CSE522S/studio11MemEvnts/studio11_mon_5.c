#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/eventfd.h>

#define MEM_SIZE 10

int main(int argc, char * argv[])
{
    char * write_buffer; //char buffer

    int efd1, efd2, cec1, cec2, mom1, mom2; //fds

    int numfds = 0, read_flag = 1;

    uint64_t read_efd1 = 0;
    uint64_t read_efd2 = 0;

    int retval;

    struct pollfd fds[2];

    int write_ret, read_ret; //return code for write

    if (argc == 5)
    {
        //if argument is 2 make sure the paths provided are correct. Otherwise, generate error
        if ((cec1 = open(argv[1], O_RDWR)) < 0)
        {
            perror("Incorrect cgroup.event_control path for child1\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");    
            return -1;
        }
        if ((cec2 = open(argv[3], O_RDWR)) < 0)
        {
            perror("Incorrect cgroup.event_control path for child2\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");    
            return -1;
        }        
        if ((mom1 = open(argv[2], O_RDONLY)) < 0)
        {
            perror("Incorrect memory.oom_contrl path for child1\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");    
            return -1;
        }
        if ((mom2 = open(argv[4], O_RDONLY)) < 0)
        {
            perror("Incorrect memory.oom_contrl path for child2\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");    
            return -1;
        }

    }
    else
    {
        //generate an error if number of arguments is not equal to 5
        printf("Incorrect arguments\nUsage: studi11_mon_4 <path to cgroup.event_control for child 1> <memory.oom_control for child 1>  <path to cgroup.event_control for child 2> <memory.oom_control for child 2>\n");
        return -1;
    }
    
    //create eventfd
    if ((efd1 = eventfd(0,0)) < 0)
    {
        perror("Eventfd child 1 error:");
        return -1;
    }
    if ((efd2 = eventfd(0,0)) < 0)
    {
        perror("Eventfd child 1 error:");
        return -1;
    }

    fds[0].fd = efd1;
	fds[0].events = POLLIN;
	numfds++;
	fds[1].fd = efd2;
	fds[1].events = POLLIN;
	numfds++;

    write_buffer = malloc(10 * sizeof(char)); //allocate some memory

    memset(write_buffer, 0, 10); //set memory to zero

    sprintf(write_buffer, "%d %d", efd1, mom1); //string containing event fd and memory.oom_control fd

    write_ret = write(cec1, write_buffer, 10); //write string to cec

    sprintf(write_buffer, "%d %d", efd2, mom2); //string containing event fd and memory.oom_control fd

    write_ret = write(cec2, write_buffer, 10); //write string to cec

    free(write_buffer);

	while(read_flag)
	{

		retval = poll (fds, numfds, -1); //poll indefinitely

		if (retval == -1)
		{
		   perror("poll()");
		   return retval;
	    }
		else
		{

			if(fds[0].revents & POLLIN)
			{
                read_ret = read(efd1, &read_efd1, sizeof(uint64_t));
                if (read_ret != sizeof(uint64_t))
                {
                    perror("Eventfd read:");
                    return -1;
                }
                else
                {
                    printf("Child 1 exceeded memory limit\n");
                    fds[0].fd = -1;
                }
            }
			if(fds[1].revents & POLLIN)
			{
                read_ret = read(efd2, &read_efd2, sizeof(uint64_t));
                if (read_ret != sizeof(uint64_t))
                {
                    perror("Eventfd read:");
                    return -1;
                }
                else
                {
                    printf("Child 2 exceeded memory limit\n");
                    fds[1].fd = -1;
                }                
            }
            if (fds[0].fd == -1 && fds[1].fd == -1) read_flag = 0;            
        }        

    }

    close(cec1);
    close(cec2);
    close(mom1);
    close(mom2);

    return 0;

}
