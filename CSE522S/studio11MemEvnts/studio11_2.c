#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define MEM_SIZE 1024

void display_mallinfo(void);
int is_string_digit(char * a);
int string_to_int(char * a);

int main(int argc, char * argv[])
{
    char * alloc_buffer; //char buffer
    int mem_size;

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
    
    alloc_buffer = malloc(mem_size * sizeof(char));

    printf("*********************\n");
    printf("Mallinfo after malloc\n");
    printf("*********************\n");

    display_mallinfo();

    free(alloc_buffer);

    printf("*********************\n");
    printf("Mallinfo after free  \n");
    printf("*********************\n");

    display_mallinfo();

    return 0;

}

/*from man 3 mallinfo*/
void display_mallinfo(void)
{
    struct mallinfo mi;

    mi = mallinfo();

    printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
    printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
    printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
    printf("# of mapped regions (hblks):           %d\n", mi.hblks);
    printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
    printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
    printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
    printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
    printf("Total free space (fordblks):           %d\n", mi.fordblks);
    printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);


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