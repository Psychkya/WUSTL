#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/eventfd.h>

#define MEM_SIZE 10

int main(int argc, char * argv[])
{
    char * write_buffer; //char buffer

    int efd, cec, mom; //fds

    uint64_t read_efd = 0;

    int write_ret, read_ret; //return code for write

    if (argc == 3)
    {
        //if argument is 2 make sure the paths provided are correct. Otherwise, generate error
        if ((cec = open(argv[1], O_RDWR)) < 0)
        {
            perror("Incorrect cgroup.event_control path\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");    
            return -1;
        }
        if ((mom = open(argv[2], O_RDONLY)) < 0)
        {
            perror("Incorrect memory.oom_contrl path\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");    
            return -1;
        }

    }
    else
    {
        //generate an error if number of arguments is not equal to 2
        printf("Incorrect arguments\nUsage: studi11_mon_4 <path to cgroup.event_control> <memory.oom_control>\n");
        return -1;
    }
    
    //create eventfd
    if ((efd = eventfd(0,0)) < 0)
    {
        perror("Eventfd error:");
        return -1;
    }

    write_buffer = malloc(10 * sizeof(char)); //allocate some memory

    memset(write_buffer, 0, 10); //set memory to zero

    sprintf(write_buffer, "%d %d", efd, mom); //string containing event fd and memory.oom_control fd

    write_ret = write(cec, write_buffer, 10); //write string to cec

    while(!read_efd)
    {
        read_ret = read(efd, &read_efd, sizeof(uint64_t));
        if (read_ret != sizeof(uint64_t))
        {
            perror("Eventfd read:");
            return -1;
        }
        else printf("Memory limit exceeded\n");
    }

    return 0;

}
