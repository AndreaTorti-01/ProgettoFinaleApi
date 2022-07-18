#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0
#define MAGIC_NUMBER 7919
#define MAXWORDLEN 32

int k;
char buffer[MAXWORDLEN];
FILE *fileptr;
FILE *wfileptr;

struct node{
    char *chunk;
    struct nodo* right;
    struct nodo* down;
};
typedef struct node* node_ptr;

uint8_t map(char c){
    if (c == '-') return (c - 45);
    else if (c >= '0' && c <= '9') return (c - 47);
    else if (c >= 'A' && c <= 'Z') return (c - 53);
    else if (c >= 'a' && c <= 'z') return (c - 59);
    else return (c - 48);
}

bool readline(){
    if (fgets(buffer, MAXWORDLEN, fileptr)){
        buffer[strcspn(buffer, "\r\n")] = '\0'; // pulisce il buffer dal newline
        return true;
    }
    else return false;
}

node_ptr trie_insert(char* word, node_ptr root){

    if (root == NULL){ // funziona solo una volta, quando il trie è vuoto
        root = malloc(sizeof(struct node));
        root->chunk = malloc(sizeof(char) * (k+1));
        strcpy(root->chunk, word);
        return root;
    }

    bool exit = false;
    node_ptr temp = root;

    while (!exit){
        if (temp == NULL){
            
        }
        else{
            
        }
    }

    return root;
}

int main(){
    node_ptr root = NULL;
    bool exit;

    readline();
    k = (int)strtol(buffer, (char **)NULL, 10); // imposta k

    exit = false;
    while (!exit){
        readline();
        if (buffer[0] != '+'){
            root = trie_insert(buffer, root);
        }
        else if ( !(strcmp(buffer, "+inserisci_inizio") == 0 || strcmp(buffer, "+inserisci_fine") == 0) )
            exit = true;
    }


    return 0;
}