#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include "read_parse_spec_file.h"

//Structure to hold lines from each file
struct Line 
{ 
    int num;
    char* content; 
};

// An AVL tree node 
struct Node 
{ 
    struct Line *key; 
    struct Node *left; 
    struct Node *right; 
    int height; 
}; 

int comparator(struct Line *p, struct Line *q);
  
// A utility function to get maximum of two integers 
int max(int a, int b); 

// A utility function to get the height of the tree 
int height(struct Node *N);

/* Helper function that allocates a new node with the given key and 
    NULL left and right pointers. */
struct Node* newNode(struct Line *key);
  
// A utility function to right rotate subtree rooted with y 
// See the diagram given above. 
struct Node *rightRotate(struct Node *y);
  
// A utility function to left rotate subtree rooted with x 
// See the diagram given above. 
struct Node *leftRotate(struct Node *x);
  
// Get Balance factor of node N 
int getBalance(struct Node *N);

// Recursive function to insert a key in the subtree rooted 
// with node and returns the new root of the subtree. 
struct Node* insert(struct Node* node, struct Line *key);

//Retrieve each value in order
void printInorder(struct Node* node, char* output_buffer, int *output_ptr);


//Call AVL insert function
void sort_buffer_AVL(char *input_buffer, char *output_buffer, int read_buff_len);


void merge(struct Line* arr1[], struct Line* arr2[],struct Line *mergedArr[], int m, int n);
