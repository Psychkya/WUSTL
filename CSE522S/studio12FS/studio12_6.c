#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <limits.h>
#define BUF_LEN 4096

void get_mask_desc(char * s, uint32_t i);

int main(int argc, char * argv[])
{
    int ifd; //fd for inotify
    int wfd[10]; // array of fd for inotify_add_watch
    int wfd_count;
    char mask_desc[1024];
    char file_path[PATH_MAX]; //get absolute file name
    char watch_list[1000][PATH_MAX]; //sparse array to hold watch list
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

    wfd[0] = inotify_add_watch(ifd, argv[1], IN_ALL_EVENTS); //add the file path given in cmd line to watch list
    if (wfd[0] < 0){
        printf("Could not add watch to %s\n", argv[1]);
        perror("Error:");
        return -1;
    }
    strcpy(watch_list[wfd[0]], argv[1]); //store the file watched in a sparse array
 
    printf("Watch added for %s; watch fd is %d\n", argv[1], wfd[0]);

    wfd_count = 1; //count to iterate over a max of 10 file or dir watches
    while(1){

        printf("**********************************\n");
        printf("****Event watch for all events****\n");
        printf("**********************************\n");

        len = read(ifd, buf, BUF_LEN); //read inotify fd
        pos = 0; //set index position to 0

        while(pos < len){

            event = (struct inotify_event *)&buf[pos]; //starting from pos, move memory area to event
            //**print event values
            printf("Event watch descriptor: %d\n", event->wd);
            printf("Event mask            : %x\n", event->mask);

            memset(mask_desc,0,1024); //reset memory
            get_mask_desc(mask_desc, event->mask); //get mask description

            printf("Event mask description: %s\n", mask_desc);    
            printf("Event cookie          : %d\n", event->cookie);
            printf("Event name field len  : %d\n", event->len);
            if (event->len) printf("Event file name       : %s\n", event->name); //if len is non-zero, print file name
            //**end of print
            pos += sizeof (struct inotify_event) + event->len; //bump pos to start of next event
            if(((event->mask & 0x00000F00) == IN_CREATE ) && wfd_count < 10) //check if a new file or dir is created
            {
                //get the full path of the file just created. since watched files are stored in sparse array indexed by watched fd, 
                //we can get the full path by concatenating the watched path to the event name
                sprintf(file_path, "%s/%s", watch_list[event->wd], event->name); 
                wfd[wfd_count++] = inotify_add_watch(ifd, file_path, IN_ALL_EVENTS); //add to watch
                if (wfd[wfd_count - 1] < 0){
                    printf("Could not add watch to %s\n", file_path);
                    perror("Error:");
                }
                else {                
                    strcpy(watch_list[wfd[wfd_count - 1]], file_path); //store the new file or dir in sparse array
                    printf("Watch added for %s; watch fd is %d\n", file_path, wfd[wfd_count - 1]);
                }
            }
        }
       
    }

    return 0;

}

void get_mask_desc(char *s, uint32_t i)
{
    //switch cases to get event mask descriptions.
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
