#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define MEM_SIZE 10

void display_mallinfo(void);
int is_string_digit(char * a);
int string_to_int(char * a);

int main(int argc, char * argv[])
{
    char * alloc_buffer; //char buffer

    char * no_init_buffer; //un-initialized buffer

    int m_check_value;

    if (argc == 2)
    {
        if (is_string_digit(argv[1]) < 0)
        {
            printf("Non numeric argument\nUsage: studi11_3 <MALLOC_CHECK vlaue - valid values are 0 to 2, default is 1>\n");
            return -1;
        }
        m_check_value = string_to_int(argv[1]);
        if (m_check_value < 0 || m_check_value > 2)
        {
            printf("Invalid MALLOC_CHECK value\nUsage: studi11_3 <MALLOC_CHECK vlaue - valid values are 0 to 2, default is 1>\n");
            return -1;            
        }
    }
    else if (argc > 2)
    {
            printf("Too many argument\nUsage: studi11_3 <MALLOC_CHECK vlaue - valid values are 0 to 2, default is 1>\n");
            return -1;
    }
    else
    {
        m_check_value = 1;
    }
    
    if(!(mallopt(M_CHECK_ACTION, m_check_value)))
    {
        printf("mallopt failed\n");
        return -1;
    }

    alloc_buffer = malloc(MEM_SIZE * sizeof(char));

    printf("****Freeing buffer allocated with malloc****\n");

    free(alloc_buffer); //first free

    printf("\n****Freeing second time buffer allocated with malloc****\n");

    free(alloc_buffer); //second free

    printf("\n****Freeing uninitialized buffer****\n");

    free(no_init_buffer); //freeing uninitialized buffer

    printf("\n****Re-alloc uninitialized buffer****\n");

    no_init_buffer = realloc(no_init_buffer, MEM_SIZE * sizeof(char));

    printf("\n****Writing to freed/uninitialized buffer****\n");

    // strcpy(alloc_buffer, "This is a test\n");
    strcpy(no_init_buffer, "This is a test\n");

    printf("output: %s", no_init_buffer);

    printf("\n****Allocate mem and write past boundary****\n");

    alloc_buffer = malloc(MEM_SIZE * sizeof(char));

    strcpy(alloc_buffer, "This is a test 123456\n");

    printf("output: %s", alloc_buffer);

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