#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/inotify.h>

int main(int argc, char * argv[])
{
    int ifd; //fd for inotify
    int wfd; //fd for inotify_add_watch

    if (argc != 2){
        printf("Too many or too few arguments\nUsage: studio12_2 <path to file or dir>\n");
        return -1;
    }

    ifd = inotify_init1(0);

    if (ifd < 0){
        perror("Inotify init error:");
        return -1;
    }

    printf("Inotify fd obtained: %d\n", ifd);

    wfd = inotify_add_watch(ifd, argv[1], IN_ALL_EVENTS);
    if (wfd < 0){
        printf("Could not add watch to %s\n", argv[1]);
        perror("Error:");
        return -1;
    }
 
    printf("Watch added for %s; watch fd is %d\n", argv[1], wfd);

    return 0;

}
