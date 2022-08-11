// fastest reading: fgets; fastest writing: printf
// il packing con  __attribute__((__packed__)) funziona

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct __attribute__((__packed__)) node{
    struct node* right;
    struct node* down;
    char *chunk;
    bool valid;
};
typedef struct node* node_ptr;

int main(){
    if (sizeof(struct node) == 25) return 1;
    return 0;
}