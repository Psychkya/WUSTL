/**************************************************************************/
/*A naive solution: Initialize an array of process with current processes */
/*available in /proc. Array size will be equal to PROC_NUM which is set at*/
/*32678. This is a sparse array where pids will be the index of the array.*/
/*If a pid exist, a simple 'y' will be moved as value of the array.       */
/*In an inifite loop, every second, a second array will be constructed the*/
/*same way. The two arrays will be compared. If the first array had a 'y' */
/*and the second did not, this means the process was destroyed. If the    */
/*second array had a 'y' and the first did not, this means a process was  */
/*created. After the comparison, the second array is copied to the first  */
/*and the second is set to 0 to start all over again.                     */
/**************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#define BUF_LEN 4096
#define PROC_NUM 32678 //max pid

int is_string_digit(char * a);
int string_to_int(char * a);

int main(int argc, char * argv[])
{
    char dir_path[PATH_MAX]; //hold dir path
    char proc_queue_1[PROC_NUM]; //sparse queue to hold process info
    char proc_queue_2[PROC_NUM]; //sparse queue to hold process info
    struct dirent *dir_info; //dir entry info
    DIR * dr; //pointer to directory type;
    struct stat find_dir; //stat the path to see if it is a dir
    int dir_index; //index to traverse the process queues

    dr = opendir("/proc"); //returns pointer to dir

    if (dr == NULL){
        printf("Could not open /proc\n");
        return -1;
    }

    memset(proc_queue_1,0,PROC_NUM); //set all values to zero
    memset(proc_queue_2,0,PROC_NUM); //set all values to zero

    /*call readdir to to get collection of directories*/
    while ((dir_info = readdir(dr)) != NULL){
        if (strcmp(dir_info->d_name, ".") != 0 && strcmp(dir_info->d_name, "..") != 0){ //do not want . and .. directories
            sprintf(dir_path,"/proc/%s", dir_info->d_name); //construct the path
            stat(dir_path, &find_dir); //call stat to find if the path is directory or file
            if (S_ISDIR(find_dir.st_mode) && is_string_digit(dir_info->d_name)) //if directory, add it to watch list
            {
                /*use dir name, which is pid, as index and move 'y' indicating process exist*/
                proc_queue_1[string_to_int(dir_info->d_name)] = 'y';
            }
        }
    }

    /*infinite loop to check for process creation and destruction*/
    while(1){
        dr = opendir("/proc"); //returns pointer to dir
        while ((dir_info = readdir(dr)) != NULL){
            if (strcmp(dir_info->d_name, ".") != 0 && strcmp(dir_info->d_name, "..") != 0){ //do not want . and .. directories
                sprintf(dir_path,"/proc/%s", dir_info->d_name); //construct the path
                stat(dir_path, &find_dir); //call stat to find if the path is directory or file
                if (S_ISDIR(find_dir.st_mode) && is_string_digit(dir_info->d_name)) //if directory, add it to watch list
                {
                    /*use dir name, which is pid, as index and move 'y' indicating process exist*/
                    proc_queue_2[string_to_int(dir_info->d_name)] = 'y';
                }
            }
        }
        /*compare the two queue to see what is added and what is destroyed*/
        for (dir_index = 0; dir_index < PROC_NUM; dir_index++)
        {
            /*if values of this index is different, something was added or destroyed*/
            if(proc_queue_1[dir_index] != proc_queue_2[dir_index]) 
            {
                if(proc_queue_1[dir_index] == 'y') printf("Process %d was destroyed\n", dir_index);
                if(proc_queue_2[dir_index] == 'y') printf("Process %d was created\n", dir_index);
            }
        }
        memcpy(proc_queue_1,proc_queue_2,PROC_NUM); //make the new queue as the first queue to compare
        memset(proc_queue_2,0,PROC_NUM); //reset the second queue
        sleep(1); //sleep for a second

    }


    return 0;

}

int is_string_digit(char* a)
{
	int result = -1;
	int len = strlen(a);
	for (int i = 0; i < len; i++)
	{
		if(a[i] >= '0' && a[i] <= '9')
		{
			result = 1;
		}
		else
		{
			result = 0;
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

