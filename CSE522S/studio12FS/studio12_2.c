#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/inotify.h>

int main(int argc, char * argv[])
{
    int ifd;

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
    return 0;

}
