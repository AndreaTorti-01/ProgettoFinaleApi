// fastest reading: fgets; fastest writing: printf

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

struct node{
    struct node* right;
    struct node* down;
    char *chunk;
    bool valid;
};
typedef struct node* node_ptr;

int main(){
    printf("%ld\n", sizeof(bool));

    return 0;
}