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
        strcpy(root->chunk, word);
        return root;
    }

    node_ptr prev = NULL;
    node_ptr temp = root;
    char *wordRemainder, *trieRemainder;
    int nMatch;

    while(1){
        nMatch = match_until(temp->chunk, word);
        
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
            temp->down->chunk = trieRemainder;
            temp->down->right = malloc(sizeof(struct node));
            temp->down->right->down = temp->down->right->right = NULL;
            temp->down->right->chunk = wordRemainder;
            return root;
        }
        else if (nMatch == -1){ // se invece corrisponde tutto dobbiamo solo inserire il wordRemainder al livello sotto
            wordRemainder = malloc(sizeof(char) * (strlen(word) - strlen(temp->chunk) + 1));
            slice(word, wordRemainder, strlen(temp->chunk), strlen(word)+1);
            trie_insert(wordRemainder, temp->down);
            free(wordRemainder);
            return root;
            
        }
        else{ // se non riesce a inserire
            prev = temp;
            temp = temp->right; // scorre a destra
            if (temp->chunk[0] > word[0]){ // se è il posto giusto dove inserire
                temp = malloc(sizeof(struct node));
                temp->right = prev->right;
                temp->down = NULL;
                temp->chunk = malloc(sizeof(char) * (strlen(word) + 1));
                strcpy(temp->chunk, word);
                prev->right = temp;
                return root;
            }
            else if (temp == NULL){ // se è alla fine
                temp = malloc(sizeof(struct node));
                temp->right = temp->down = NULL;
                temp->chunk = malloc(sizeof(char) * (strlen(word) + 1));
                strcpy(temp->chunk, word);
                prev->right = temp;
                return root;
            }
            
        }
    }
}

void print_trie(char* passed, int index, node_ptr root){
    if (root->down == NULL){ // se sono in una foglia
        printf("%s%s\n", passed, root->chunk);
        if (root->right != NULL) 
            print_trie(passed, index, root->right);
    }
    else{ // se sono in mezzo
        if (root->right != NULL) 
            print_trie(passed, index, root->right);
        cut_end(passed, index);
        passed = strcat(passed, root->chunk);
        index += strlen(root->chunk);
        print_trie(passed, index, root->down);
    }
}

int main(){
    node_ptr root = NULL;
    int i;
    char strings[5][5] = {"crac", "rato", "crer", "raro", "clir"};
    char init[] = "\0\0\0\0";
    for (i=0; i<(sizeof(strings)/sizeof(strings[0])); i++)
        root = trie_insert(strings[i], root);
    print_trie(init, 0, root);

    return 0;
}