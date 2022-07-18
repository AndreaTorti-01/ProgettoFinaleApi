#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0


struct node{
    char *chunk;
    struct node* right;
    struct node* down;
};
typedef struct node* node_ptr;

void slice(char *str, char *result, size_t start, size_t end){
    strncpy(result, str + start, end - start);
}

// ritorna la lunghezza del pezzo iniziale che corrisponde, 0 se non corrisponde, -1 se corrisponde fino alla fine di trieChunk
int match_until(char* trieChunk, char* inputWord){
    int num;
    int max = strlen(trieChunk);
    for(num = 0; num < max && inputWord[num] == trieChunk[num]; num++);
    if (num == max) return -1;
    return num;
}

node_ptr trie_insert(char* word, node_ptr root){
    
    if (root == NULL){
        root = malloc(sizeof(struct node));
        root->chunk = malloc(sizeof(char) * (strlen(word) + 1));
        root->down = root->right = NULL;
        strcpy(root->chunk, word);
        return root;
    }

    node_ptr prev = NULL;
    node_ptr temp = root;
    char *wordRemainder, *trieRemainder;
    int nMatch;

    while(temp != NULL){
        nMatch = match_until(temp->chunk, word);
        
        if (nMatch > 0){ // deve inserire in temp->down wordRemainder e trieRemainder, ovvero le parti che non corrispondono
            wordRemainder = malloc(sizeof(char) * (strlen(word) - nMatch + 1));
            trieRemainder = malloc(sizeof(char) * (strlen(temp->chunk) - nMatch + 1));
            slice(word, wordRemainder, nMatch, strlen(word)); // mette in wordRemainder i caratteri di word che non corrispondevano
            slice(temp->chunk, trieRemainder, nMatch, strlen(temp->chunk)); // mette in trieRemainder i caratteri di chunk che non corrispondevano

            temp->chunk = realloc(temp->chunk, sizeof(char) * (nMatch+1)); // rimpicciolisce la parola già presente nel trie
            temp->chunk[nMatch] = '\0';
            temp->down = malloc(sizeof(struct node));  // inserisce il pezzo rimasto sotto
            temp->down->down = temp->down->right = NULL;
            temp->down->chunk = trieRemainder;

            temp = trie_insert(wordRemainder, temp->down); // non rimane che inserire ciò che è rimasto al livello sotto
            return root;
        }
        else if (nMatch == -1){ // se invece corrisponde tutto dobbiamo solo inserire il wordRemainder al livello sotto
            wordRemainder = malloc(sizeof(char) * (strlen(word) - strlen(temp->chunk) + 1));
            slice(word, wordRemainder, strlen(temp->chunk), strlen(word));
            temp = trie_insert(wordRemainder, temp->down);
            return root;
            
        }
        else{ // se non riesce a inserire
            prev = temp;
            temp = temp->right; // scorre a destra
        }
    }

    // inserisce l'ultimo pezzo
    temp = malloc(sizeof(struct node));
    if (prev != NULL) prev->right = temp;
    temp->chunk = malloc(sizeof(char) * (strlen(word) + 1));
    temp->down = temp->right = NULL;
    strcpy(temp->chunk, word);
    return root;
}

int main(){
    node_ptr root = NULL;
    char string1[] = "ciao";
    char string2[] = "capa";
    char string3[] = "caro";
    char string4[] = "rapa";
    char init[] = "\0\0\0\0";
    root = trie_insert(string1, root);
    root = trie_insert(string2, root);
    root = trie_insert(string3, root);
    root = trie_insert(string4, root);
    printf("%s\n", root->chunk);
    printf("%s\n", root->down->chunk);
    printf("%s\n", root->down->right->chunk);
    printf("%s\n", root->down->right->down->chunk);
    printf("%s\n", root->down->right->down->right->chunk);
    printf("%s\n", root->right->chunk);

    return 0;
}