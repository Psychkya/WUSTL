#include"sort_buffer_AVL.h"

int comparator(struct Line *p, struct Line *q)  
{ 
    int l =p->num;
    int r = q->num;
    return (l - r); 
} 

// A utility function to get the height of the tree 
int height(struct Node *N) 
{ 
    if (N == NULL) 
        return 0; 
    return N->height; 
} 
  
// A utility function to get maximum of two integers 
int max(int a, int b) 
{ 
    return (a > b)? a : b; 
} 
  
/* Helper function that allocates a new node with the given key and 
    NULL left and right pointers. */
struct Node* newNode(struct Line *key) 
{ 
    struct Node* node = (struct Node*) 
                        malloc(sizeof(struct Node)); 
    node->key   = key; 
    node->left   = NULL; 
    node->right  = NULL; 
    node->height = 1;  // new node is initially added at leaf 
    return(node); 
} 
  
// A utility function to right rotate subtree rooted with y 
// See the diagram given above. 
struct Node *rightRotate(struct Node *y) 
{ 
    struct Node *x = y->left; 
    struct Node *T2 = x->right; 
  
    // Perform rotation 
    x->right = y; 
    y->left = T2; 
  
    // Update heights 
    y->height = max(height(y->left), height(y->right))+1; 
    x->height = max(height(x->left), height(x->right))+1; 
  
    // Return new root 
    return x; 
} 
  
// A utility function to left rotate subtree rooted with x 
// See the diagram given above. 
struct Node *leftRotate(struct Node *x) 
{ 
    struct Node *y = x->right; 
    struct Node *T2 = y->left; 
    // Perform rotation 
    y->left = x; 
    x->right = T2; 
  
    //  Update heights 
    x->height = max(height(x->left), height(x->right))+1; 
    y->height = max(height(y->left), height(y->right))+1; 
  
    // Return new root   
    return y; 
} 
  
// Get Balance factor of node N 
int getBalance(struct Node *N) 
{ 
    if (N == NULL) 
        return 0; 
    return height(N->left) - height(N->right); 
} 
  
// Recursive function to insert a key in the subtree rooted 
// with node and returns the new root of the subtree. 
struct Node* insert(struct Node* node, struct Line *key) 
{ 
    /* 1.  Perform the normal BST insertion */
    if (node == NULL) 
        return(newNode(key)); 
	if(comparator(key,node->key)<0)
		node->left  = insert(node->left, key); 
	else if(comparator(key,node->key)>0)
		node->right = insert(node->right, key); 
	else 
		return node;
    /* 2. Update height of this ancestor node */
    node->height = 1 + max(height(node->left), 
                           height(node->right)); 
  
    /* 3. Get the balance factor of this ancestor 
          node to check whether this node became 
          unbalanced */
    int balance = getBalance(node); 
  
    // If this node becomes unbalanced, then 
    // there are 4 cases 
	if (balance > 1 && comparator(key,node->left->key) <0) 
        return rightRotate(node); 
	if (balance < -1 && comparator(key,node->right->key) >0) 
        return leftRotate(node); 
	if (balance > 1 && comparator(key,node->left->key) >0) 
    { 
        node->left =  leftRotate(node->left); 
        return rightRotate(node); 
    } 
	if (balance < -1 && comparator(key,node->right->key) <0) 
    { 
        node->right = rightRotate(node->right); 
        return leftRotate(node); 
    } 

    /* return the (unchanged) node pointer */
    return node; 
} 



void printInorder(struct Node* node, char* output_buffer, int *output_ptr) 
{ 
    if (node == NULL) 
        return; 
  
    /* first recur on left child */
    printInorder(node->left, output_buffer, output_ptr); 
  
    printf("content len: %d, content:  %s\n", strlen(node->key->content), node->key->content);
    
    strncpy(output_buffer + *output_ptr, node->key->content, strlen(node->key->content) );
    (*output_ptr) += strlen(node->key->content);

    printf("out ptr: %d, out: %s", *output_ptr, output_buffer);
    /* now recur on right child */
    printInorder(node->right, output_buffer, output_ptr); 
} 


void sort_buffer_AVL(char *input_buffer, char *output_buffer, int buffer_len)
{
    struct Node *root = NULL; 
    
    int parse_len = 0, ptr_loc = 0, output_ptr = 0;
    int count=0;
    struct Line** ptr;
    ptr = malloc(10 * sizeof(struct Line*)); //starting with 10 lines per file
    char* p_buffer = malloc(100*sizeof(char));
    while (parse_len < buffer_len && parse_len > -1)
    {

        if (count >= 10)
        {
            ptr= realloc(ptr, (count + 10 * 2) * sizeof(struct Line)); 
        }
        
        //memset(p_buffer, 0, sizeof(p_buffer));
        parse_len = parse_buffer(input_buffer, &p_buffer, buffer_len, ptr_loc); //ptr_loc provides next relative position of pointer
        ptr_loc += parse_len;
        printf("l:%s",p_buffer);
        
        int n = -1;
        ptr[count]= (struct Line*)malloc(sizeof(struct Line)); 
        int read = sscanf(p_buffer, "%d", &n);
        char* string = malloc(strlen(p_buffer) * sizeof(char));
        //= (char *) malloc(128);;
        strcpy(string, p_buffer);
        ptr[count]->num = n;
        ptr[count]->content = string;
        root = insert(root, ptr[count]); 
        count++;
        if (buffer_len - ptr_loc <= 0) break;

    }    
    printInorder(root, output_buffer, &output_ptr);
    

}
