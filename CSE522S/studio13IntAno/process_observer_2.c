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
    FILE *fp_cmd; //file pointer to pid/status file
    FILE *fp_stat; //file pointer to pid/status file
    char stat_buffer[100]; //buffer to hold status file line
    char stat_file[50]; //hold full file path of status file
    char get_file_name[50]; //holds the parent pid
    char parent_child_buffer[4096]; //buffer to print heirarchy
    char *PPid; //stores parent pid
    int recursive_read = 1; //flag to enable recursive read
    int stop_fgets = 0; //flag to stop reading status file
    int first_time = 1; //flag to control printing of parent child heirarchy
    int print_flag = 0; //flag galore!! decide if we need to print parent child heirarchy
    
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
                first_time = 1;
                if(proc_queue_1[dir_index] == 'y') printf("Process %d was destroyed\n", dir_index);
                if(proc_queue_2[dir_index] == 'y') {
                    /*if new process, print the parent child heirarcy*/
                    printf("Process %d was created\n", dir_index);
                    memset(get_file_name,0,50);
                    sprintf(get_file_name, "%d", dir_index); //convert index to string to construct file path                   
                    while(recursive_read){
                        /*start reading pid dirs until original parent is found*/
                        stop_fgets = 0; //reset flag
                        sprintf(stat_file, "/proc/%s/cmdline", get_file_name); //construct cmdline path
                        fp_cmd = fopen(stat_file, "r"); //open cmdline
                        if (fp_cmd == NULL) {
                            /*if null drop out of recursion*/
                            perror(stat_file);
                            recursive_read = 0;
                        }
                        else{
                            /*otherwise read cmdline file*/
                            fgets(stat_buffer,100, fp_cmd); 
                            if(strcmp(stat_buffer,"./anomalous_process") == 0)
                            {
                                /*if it is the program we are interested in*/
                                if (first_time) {
                                    /*construct the first line of the print buffer*/
                                    strcpy(parent_child_buffer, get_file_name);
                                    first_time = 0;
                                    print_flag = 1;
                                }
                                else {
                                    /*else continue apending prior parent*/
                                    strcat(parent_child_buffer, "<-");
                                    strcat(parent_child_buffer, get_file_name);
                                    print_flag = 1;
                                }

                                sprintf(stat_file,"/proc/%s/status", get_file_name); //construct status file path
                                fp_stat = fopen(stat_file, "r"); //read status file
                                if (fp_stat == NULL) {
                                    /*if null, drop out of recursion*/
                                    perror(stat_file);
                                    recursive_read = 0;
                                }
                                else{
                                    /*otherwise read status file*/
                                    while(!stop_fgets){
                                        if (fgets(stat_buffer, 100, fp_stat) == NULL) {
                                            /*if for some odd reason ppid is not found*/
                                            stop_fgets = 1;
                                            recursive_read = 0;
                                        }
                                        else {
                                            /*continue reading and drop out once ppid is found*/        
                                            PPid = strtok(stat_buffer, " :"); //splits string on space and :
                                            if(strcmp(PPid,"PPid") == 0)
                                            {
                                                stop_fgets = 1; //ppid found so set flag to drop out
                                                PPid = strtok(NULL, " :\t"); //split on space, : and tab
                                                /*remove the newline character - this will be parent ppid dir to read*/
                                                memset(get_file_name,0,50);
                                                strncpy(get_file_name,PPid, strlen(PPid) - 1); 
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                /*if not our program, do not recurse anymore*/
                                recursive_read = 0;
                            }
                            /*close files appropriately*/
                            if (fp_cmd != NULL) {
                                fclose(fp_cmd);
                                fp_cmd = NULL;
                            }
                            if (fp_stat != NULL){
                                fclose(fp_stat);
                                fp_stat = NULL;
                            }
                        }   
                    }
                    /*once recused through all dirs, print the buffer*/
                    if(print_flag) {
                        printf("%s\n", parent_child_buffer);
                        memset(parent_child_buffer,0,2048);
                        print_flag = 0;
                    }
                    recursive_read = 1;
                    stop_fgets = 0;

                }
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

