#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_START_SIZE 64
int k;

unsigned long long HashMultiplicative(const char key[], int keyLen) {
   unsigned long long hash = 0;
   for(int i = 0; i < keyLen; ++i)
      hash = 31 * hash + key[i];
   return hash % table_size;
}

void rehash() {
    for (int i=0; i<table_size; i++)
        Insert(hashTable[i], len(hashTable[i]));
}

void Insert(const char key[], int keyLen) {
    if (dict_size > table_size/2){
        hashTable = (node_t**) realloc(hashTable, (table_size*2) * sizeof(node_t*));
        rehash();
        table_size*=2;
    }
    unsigned long long hash = HashMultiplicative(key, keyLen);
    node_t* newNode = (node_t *) malloc(sizeof(node_t));
    strcpy(newNode->word, key);
    newNode->next = NULL;
    hashTable[hash] = newNode;
    dict_size++;
}

void printArray (int Array[], int size) {
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", Array[i]);
    printf("\n");
}

int main() {
    //open file
    FILE *fileptr = fopen("input.txt", "r");
    if (fileptr == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    char buffer[k]
    //read first line to get number of chars
    k = (int)fgets(buffer, 3, fileptr)

    //read file line by line
    while (strcmp(fgets(buffer, k+1, fileptr), "+nuova_partita") != 0) {
        //insert into hash table
        Insert(buffer, strlen(buffer));
    }
}