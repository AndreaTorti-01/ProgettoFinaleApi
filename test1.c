#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAGIC_NUMBER 31
#define MAXWORDLEN 128
#define TABLESIZE 16

int k;

typedef struct node{
    char* word;
    bool valid;
    struct node* next;
} elem;
typedef elem* elem_ptr;

int MultHash(char* key) {
   int hash = 0;
   for(int i = 0; i < k; ++i)
      hash = MAGIC_NUMBER * hash + key[i];
   return hash%TABLESIZE;
}

elem_ptr head_insert(elem_ptr head, char* wordInput){
    elem_ptr temp;
    temp = (elem_ptr) malloc(sizeof(elem));
    if (temp != NULL){
        temp->next = head;
        temp->word = (char*) malloc(sizeof(char) * (k + 1));
        strcpy(temp->word, wordInput);
        temp->valid = true;
        head = temp;
    } else printf("\nErrore di allocazione.");
    return head;
}

elem_ptr tail_insert(elem_ptr head, char* wordInput){   /*per creare una lista da zero è importante inizializzare la testa a null*/ 
    if (head == NULL) return head_insert(head, wordInput);
    head->next = tail_insert(head->next, wordInput);
    return head;
}

void visualizza(elem_ptr head){
    for (; head != NULL; head = head->next) printf("%s -> ", head->word);
    printf("NULL\n");
}

int main(){
    char buffer[MAXWORDLEN];
    elem_ptr* list;
    int hash, i;

    list = (elem_ptr*) malloc(sizeof(elem_ptr) * TABLESIZE);
    for (i=0; i<TABLESIZE; i++)
        list[i] = NULL;

    FILE* fileptr = fopen("input.txt", "r");
    k = (int)(fgets(buffer, MAXWORDLEN, fileptr)[0] - '0');
    
    while ((fgets(buffer, MAXWORDLEN, fileptr)) != NULL){
        buffer[strcspn(buffer, "\r\n")] = 0;

        hash = MultHash(buffer);

        list[hash] = tail_insert(list[hash], buffer);
    }

    for (i=0; i<TABLESIZE; i++)
        visualizza(list[i]);

    return 0;
}
