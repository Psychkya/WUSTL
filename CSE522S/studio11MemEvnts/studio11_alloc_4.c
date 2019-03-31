#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define MEM_SIZE 102400

int is_string_digit(char * a);
int string_to_int(char * a);

int main(int argc, char * argv[])
{
    char * alloc_buffer; //char buffer
    long mem_size;
    long mem_counter = 0;

    if (argc == 2)
    {
        if (is_string_digit(argv[1]) < 0)
        {
            printf("Non numeric argument\nUsage: studi11_2 <mem size in kb>\n");
            return 0;
        }
        mem_size = string_to_int(argv[1]) * 1024;
    }
    else if (argc > 2)
    {
        printf("Too many arguments\nUsage: studi11_2 <mem size in kb>\n");
        return 0;
    }
    else
    {
        mem_size = MEM_SIZE;
    }
    
    while (mem_counter <= mem_size)
    {
        mem_counter += 2048;
        printf("Allocated %ld bytes of memory\n", mem_counter);
        alloc_buffer = malloc(mem_counter * sizeof(char));

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
			result = 0;
		}
		else
		{
			result = -1;
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