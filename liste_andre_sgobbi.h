#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct N {
    int val;
    struct N *next;
} node_t;

typedef node_t* node_ptr;

void visualizza(node_ptr);
int list_len(node_ptr);
node_ptr find_elem(node_ptr, int);
int elem_in_list(node_ptr, int);
node_ptr head_insert(node_ptr, int);
node_ptr tail_insert(node_ptr, int);
node_ptr remove_head(node_ptr);
node_ptr remove_elem(node_ptr, int);
void delete_list(node_ptr);
node_ptr ordered_insert(node_ptr, int);

void visualizza(node_ptr head){
    for (; head != NULL; head = head->next) printf("%d -> ", head->val);
    printf("NULL\n");
}

int list_len(node_ptr head){
    int i;
    for (i = 0; head != NULL; head = head->next) ++i;
    return i;
}

node_ptr find_elem(node_ptr head, int n){
    for (; head != NULL && head->val != n; head = head->next);
    return head;
}

int elem_in_list(node_ptr head, int n){
    for (; head != NULL; head = head->next){
        if (head->val == n) return 1;
    }
    return 0;
}

node_ptr head_insert(node_ptr head, int n){
    node_ptr temp;
    temp = (node_ptr) malloc(sizeof(node_t));
    if (temp != NULL){
        temp->next = head;
        temp->val = n;
        head = temp;
    } else printf("\nErrore di allocazione.");
    return head;
}

node_ptr tail_insert(node_ptr head, int n){   /*per creare una lista da zero è importante inizializzare la testa a null*/ 
    if (head == NULL) return head_insert(head, n);
    head->next = tail_insert(head->next, n);
    return head;
}

node_ptr remove_head(node_ptr head){
    node_ptr temp;
    if (head != NULL){
        temp = head;
        head = head->next;
        free(temp);
    }
    return head;
}

node_ptr remove_elem(node_ptr head, int n){
    node_ptr curr, prev;
    int found;
    prev = NULL;
    found = 0;
    for (curr = head; curr != NULL && !found; curr = curr->next){
        if (curr->val == n){
            found = 1;
            if (prev == NULL) {
                head = remove_head(head);
            } else {
                curr = remove_head(curr);
                prev->next = curr;
            }
        }
        prev = curr;
    }
    return head;
}

void delete_list(node_ptr head){
    if (head == NULL) return;
    delete_list(head->next);
    free(head);
}

node_ptr ordered_insert(node_ptr head, int n){
    if (head==NULL || head->val>n){
        return head_insert(head, n);
    }
    head->next=ordered_insert(head->next, n);
    return head;
}