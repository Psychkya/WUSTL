#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/inotify.h>
#define BUF_LEN 4096

void get_mask_desc(char * s, uint32_t i);

int main(int argc, char * argv[])
{
    int ifd; //fd for inotify
    int wfd; //fd for inotify_add_watch
    char mask_desc[1024];
    ssize_t len; //len from each notify read
    ssize_t pos; //index position
    char buf[BUF_LEN] __attribute__((aligned(__alignof__(struct inotify_event)))); //char buffer to hold events
    struct inotify_event *event; //pointer to inotify_event struct

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

    while(1){

        len = read(ifd, buf, BUF_LEN);
        pos = 0;
        while(pos < len){

            event = (struct inotify_event *)&buf[pos];
            printf("Event watch descriptor: %d\n", event->wd);
            printf("Event mask            : %x\n", event->mask);

            memset(mask_desc,0,50);
            get_mask_desc(mask_desc, event->mask); //get mask description

            printf("Event mask description: %s\n", mask_desc);    
            printf("Event cookie          : %d\n", event->cookie);
            printf("Event name field len  : %d\n", event->len);
            if (event->len) printf("Event file name       : %s\n", event->name);
            pos += sizeof (struct inotify_event) + event->len;
        }
    }

    return 0;

}

void get_mask_desc(char *s, uint32_t i)
{
    int m = i & 0x0000000F;
    switch(m)
    {
        case 0x00000001: 
            strcat(s,"IN_ACCESS|");
            break;
        case 0x00000002: 
            strcat(s,"IN_MODIFY|");
            break;
        case 0x00000004: 
            strcat(s,"IN_ATTRIB|");
            break;
        case 0x00000008: 
            strcat(s,"IN_CLOSE_WRITE|");
            break;
        // default: strcat(s,"");
    }

    m = i & 0x000000F0;
    switch(m)
    {
        case 0x00000010: 
            strcat(s,"IN_CLOSE_NOWRITE|");
            break;
        case 0x00000020: 
            strcat(s,"IN_OPEN|");
            break;
        case 0x00000040: 
            strcat(s,"IN_MOVED_FROM|");
            break;
        case 0x00000080: 
            strcat(s,"IN_MOVED_TO|");
            break;
        // default: strcat(s,"");
    }    

    m = i & 0x00000F00;
    switch(m)
    {
        case 0x00000100: 
            strcat(s,"IN_CREATE|");
            break;
        case 0x00000200: 
            strcat(s,"IN_DELETE|");
            break;
        case 0x00000400: 
            strcat(s,"IN_DELETE_SELF|");
            break;
        case 0x00000800: 
            strcat(s,"IN_MOVE_SELF|");
            break;
        // default: strcat(s,"");
    }    

    m = i & 0x0000F000;
    switch(m)
    {
        case 0x00002000: 
            strcat(s,"IN_UMOUNT|");
            break;
        case 0x00004000: 
            strcat(s,"IN_Q_OVERFLOW|");
            break;
        case 0x00008000: 
            strcat(s,"IN_IGNORED|");
            break;
        // default: strcat(s,"");
    }    

    m = i & 0x0F000000;
    switch(m)
    {
        case 0x01000000: 
            strcat(s,"IN_ONLYDIR|");
            break;
        case 0x02000000: 
            strcat(s,"IN_DONT_FOLLOW|");
            break;
        case 0x04000000: 
            strcat(s,"IN_EXEC_UNLINK|");
            break;
        // default: strcat(s,"");
    }

    m = i & 0xF0000000;
    switch(m)
    {
        case 0x10000000: 
            strcat(s,"IN_MASK_CREATE|");
            break;
        case 0x20000000: 
            strcat(s,"IN_MASK_ADD|");
            break;
        case 0x40000000: 
            strcat(s,"IN_ISDIR|");
            break;
        case 0x80000000: 
            strcat(s,"IN_ONESHOT|");
            break;
        // default: strcat(s,"");
    }    
}
