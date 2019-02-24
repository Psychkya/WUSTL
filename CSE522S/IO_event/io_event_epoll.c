#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#define MAX_EVENTS 50

int main(void)
{
    struct epoll_event event, events[MAX_EVENTS];
    int e_fd, nr_events;

	e_fd = epoll_create1(0);
    if (e_fd < 0) 
    {
        perror("epoll_create1()");
        return -1;
    }

    event.events = EPOLLIN;
    event.data.fd = 0;

    if((epoll_ctl(e_fd, EPOLL_CTL_ADD, 0, &event)) < 0)
    {
        perror("epoll_ctl");
        return -1;
    }


	while(1)
	{
        printf("Polling....\n");
        nr_events = epoll_wait(e_fd, events, MAX_EVENTS, -1)    ;
        if (nr_events < 0)
        {
            perror("epoll_wait");
            return -1;
        }
        printf("Data ready..Number of events: %d\n", nr_events);   

	}
	   
  
	   

   exit(EXIT_SUCCESS);
}
