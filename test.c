#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0

int k = 4;

struct node{
    struct node* right;
    struct node* down;
    char *chunk;
    bool valid;
};
typedef struct node* node_ptr;

void slice(char *str, char *result, size_t start, size_t end){
    strncpy(result, str + start, end - start);
}

void cut_end(char* word, int end){
    for (;end < strlen(word); end++)
        word[end] = '\0';
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
        root->valid = true;
        strcpy(root->chunk, word);
        return root;
    }

    node_ptr prev = NULL;
    node_ptr temp = root;
    char *wordRemainder, *trieRemainder;
    int nMatch;

    while(1){
        if (temp!= NULL) nMatch = match_until(temp->chunk, word);
        else nMatch = 0;
        
        if (nMatch > 0){ // deve inserire in temp->down wordRemainder e trieRemainder, ovvero le parti che non corrispondono
            wordRemainder = malloc(sizeof(char) * (strlen(word) - nMatch + 1));
            trieRemainder = malloc(sizeof(char) * (strlen(temp->chunk) - nMatch + 1));
            slice(word, wordRemainder, nMatch, strlen(word)+1); // mette in wordRemainder i caratteri di word che non corrispondevano
            slice(temp->chunk, trieRemainder, nMatch, strlen(temp->chunk)+1); // mette in trieRemainder i caratteri di chunk che non corrispondevano

            temp->chunk = realloc(temp->chunk, sizeof(char) * (nMatch+1)); // rimpicciolisce la parola già presente nel trie
            temp->chunk[nMatch] = '\0';
            prev = temp->down;
            temp->down = malloc(sizeof(struct node));  // inserisce il pezzo rimasto sotto
            temp->down->down = prev;
            temp->down->valid = true;
            temp->down->chunk = trieRemainder;
            temp->down = trie_insert(wordRemainder, temp->down); // ed ora inserisce il remainder della word nel livello sotto
            free(wordRemainder);
            return root;
        }
        else if (nMatch == -1){ // se invece corrisponde tutto dobbiamo solo inserire il wordRemainder al livello sotto
            wordRemainder = malloc(sizeof(char) * (strlen(word) - strlen(temp->chunk) + 1));
            slice(word, wordRemainder, strlen(temp->chunk), strlen(word)+1);
            temp->down = trie_insert(wordRemainder, temp->down);
            free(wordRemainder);
            return root;
            
        }
        else{ // se non riesce a inserire
            if (temp == NULL){ // magari è la fine della lista
                temp = malloc(sizeof(struct node));
                temp->right = temp->down = NULL;
                temp->valid = true;
                prev->right = temp;
                temp->chunk = malloc(sizeof(char) * (strlen(word) + 1));
                strcpy(temp->chunk, word);
                return root;
            }
            if (temp->chunk[0] > word[0]){ // oppure sto inserendo prima della fine
                if (prev == NULL) { // magari all'inizio
                    prev = malloc(sizeof(struct node));
                    prev->right = temp;
                    prev->down = NULL;
                    prev->valid = true;
                    prev->chunk = malloc(sizeof(char) * (strlen(word) + 1));
                    strcpy(prev->chunk, word);
                    return prev;
                }
                else{ // oppure in mezzo
                    temp = malloc(sizeof(struct node));
                    temp->right = prev->right;
                    prev->right = temp;
                    temp->down = NULL;
                    temp->valid = true;
                    temp->chunk = malloc(sizeof(char) * (strlen(word) + 1));
                    strcpy(temp->chunk, word);
                    return root;
                }
            }
            prev = temp;
            temp = temp->right; // scorre a destra
        }
    }
}

void print_trie(char* passed, int index, node_ptr root){
    if (root->down == NULL){ // se sono in una foglia
        if (root->valid) printf("%s%s\n", passed, root->chunk);
        if (root->right != NULL) 
            if (root->right->valid) print_trie(passed, index, root->right);
    }
    else{ // se sono in mezzo
        if (root->down->valid){
            char temp[k + 1];
            strcpy(temp, passed);
            cut_end(temp, index);
            strcat(temp, root->chunk);
            index += strlen(root->chunk);
            print_trie(temp, index, root->down);
        }
        if (root->right != NULL) 
            if (root->right->valid) print_trie(passed, index, root->right);
    }
}

void items_in_trie(int *num, node_ptr root){
    if (root->right != NULL)
        items_in_trie(num, root->right);
    if (root->down != NULL)
        items_in_trie(num, root->down);
    if (root->down == NULL)
        (*num)++;
}

bool is_in_trie(char* word, node_ptr root){
    int nMatch;
    char tempWord[k + 1];
    while (root != NULL){ // scorre a destra fino alla fine
        nMatch = match_until(root->chunk, word);
        if (nMatch == -1){
            if (strlen(root->chunk) == strlen(word) && root->down == NULL) return true;
            slice(word, tempWord, strlen(root->chunk), strlen(word) + 1);
            return is_in_trie(tempWord, root->down);
        }
        else if (nMatch > 0){
            slice(word, tempWord, nMatch, strlen(word) + 1);
            return is_in_trie(tempWord, root->down);
        }
        root = root->right;
    }
    return false;
}

void revalidate_trie(node_ptr root){
    if (root->down == NULL){ // se sono in una foglia
        root->valid = true;
        if (root->right != NULL) 
            revalidate_trie(root->right);
    }
    else{ // se sono in mezzo
        if (root->right != NULL) 
            revalidate_trie(root->right);
        revalidate_trie(root->down);
    }
}

int main(){
    node_ptr root = NULL;
    int i;
    char strings[8][5] = {"ciao", "capa", "caro", "capo", "rapa", "raro", "rate", "riuk"};
    char init[] = "\0\0\0\0";
    for (i=0; i<(sizeof(strings)/sizeof(strings[0])); i++)
        root = trie_insert(strings[i], root);
    print_trie(init, 0, root);

    return 0;
}