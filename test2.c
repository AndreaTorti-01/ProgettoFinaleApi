#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAGIC_NUMBER 31
#define MAXWORDLEN 128
#define TABLESIZE 16

int k;
char buffer[MAXWORDLEN];
FILE* fileptr;

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
        strncpy(temp->word, wordInput, k+1);
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

int elem_in_list(elem_ptr head, char* wordInput){
    for (; head != NULL; head = head->next){
        if (strcmp(head->word, wordInput) == 0) return 1;
    }
    return 0;
}

void visualizza(elem_ptr head){
    for (; head != NULL; head = head->next) printf("%s -> ", head->word);
    printf("NULL\n");
}

bool readline(){
    if (fgets(buffer, MAXWORDLEN, fileptr)){
        buffer[strcspn(buffer, "\r\n")] = '\0'; // pulisce il buffer dal newline
        return true;
    }
    else return false;
}

bool validateSample (char* sample, char* word, char* guesses) {
    int i, j;
    bool isValid, found;
    isValid = true;
    for (i=0; i<k && isValid; i++){ // scorre le lettere del sample e della word
        if (guesses[i] == '+'){  // se la lettera era indovinata...
            if (sample[i] == word[i]){  // ...ed è uguale a quella che stiamo leggendo
                sample[i] = '?';  // la usiamo e proseguiamo
            }
            else isValid = false;   // altrimenti invalidiamo subito
        }
        else if (guesses[i] == '|'){  // se invece era al posto sbagliato...
            if (sample[i] == word[i]) {
                isValid = false;  // non deve esserci lì!
            }
            else {
                for (j=0, found=false; j<k && !found; j++){    // scorriamo tutto il sample alla ricerca di una posizione dove c'è sta lettera
                    if (sample[j] == word[i]){  // trovata!
                        found = true;
                        sample[j] = '?';  // la usiamo
                    }
                }
                if (!found) isValid = false;
            }
        }
        else if (guesses[i] == '/'){  // se la lettera era sbagliata...
            for (j=0; j<k && isValid; j++){ // ...la cerco
                if (sample[j] == word[i]) isValid = false;  // trovata! invalido tutto
            }
        }
    }
    return isValid;
}

int main(){
    elem_ptr* list;
    elem_ptr tempHead;
    int hash, i, n, j, q, x;    // n numero di turni ancora disp, x numero parole valide
    bool exit, found;
    fileptr = fopen("opentestcases/test2.txt", "r");

    list = (elem_ptr*) malloc(sizeof(elem_ptr) * TABLESIZE);
    for (i=0; i<TABLESIZE; i++)
        list[i] = NULL;

    readline();
    k = (int)strtol(buffer, (char **)NULL, 10);  // imposta k
    char riferimento[k+1], temp[k+1], output[k+1], guessedChars[k+1];

    // popola la tabella di parole ammissibili
    exit = false;
    while (exit == false) {
        readline();
        if (buffer[0] != '+'){
            hash = MultHash(buffer);
            list[hash] = tail_insert(list[hash], buffer);
        } else exit = true;
    }

    // copia la prima parola di riferimento
    readline();
    strncpy(riferimento, buffer, k+1);

    // legge il numero massimo di parole da confrontare
    readline();
    n = (int)strtol(buffer, (char **)NULL, 10);

    while(readline()){  // in buffer è contenuta la linea letta
        if (buffer[0] == '+'){
            // esegue e stampa le parole ammissibili valide in ordine
            if (strcmp(buffer, "+stampa_filtrate") == 0){
            }

            // esegue e popola ulteriormente la lista di parole ammissibili
            else if (strcmp(buffer, "+inserisci_inizio") == 0){
            }

            else if (strcmp(buffer, "+nuova_partita") == 0){
                // rivalida tutte le parole
                for (i=0; i<TABLESIZE; i++){    // scorre linee della tabella
                    for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next){   // scorre elementi della linea
                        tempHead->valid = true;
                    }
                }

                // copia la prima parola di riferimento
                readline();
                strncpy(riferimento, buffer, k+1);

                // legge il numero massimo di parole da confrontare
                readline();
                n = (int)strtol(buffer, (char **)NULL, 10);
            }
        }

        // esegue se la parola è proprio r e può leggere parole stampa ok e termina la partita
        else if (n>0 && strcmp(buffer, riferimento) == 0){
            printf("ok\n");
            n = 0;
        }
        // esegue solo se può ancora leggere parole (e la parola non era r)
        else if (n>0){
            // esegue solo se la parola è ammissibile e la confronta con r: + ok, | semi, / no.
            hash = MultHash(buffer);
            if(elem_in_list(list[hash], buffer)){
                strncpy(temp, riferimento, k+1);
                for (i=0; i<k; i++){
                    output[i] = '/';
                    if (temp[i] == buffer[i]){
                        output[i] = '+';
                        temp[i] = '?';
                    }
                    else{
                        for (j=0; j<k; j++){
                            if (buffer[i] == temp[j]){
                                temp[j] = '?';
                                output[i] = '|';
                            }
                        }
                    }
                }
                output[k] = '\0';
                printf("%s\n", output);
                // inizia la validazione di quelle ancora valide
                x = 0;
                for (i=0; i<TABLESIZE; i++){    // scorre linee della tabella
                    for (tempHead = list[i]; tempHead != NULL && tempHead->valid; tempHead = tempHead->next){   // scorre elementi della linea se validi
                        strncpy(temp, tempHead->word, k+1);   // mette l'elemento in temp (sarà modificato!)
                        if (validateSample(temp, buffer, output)){
                            x++;
                        }   // aumenta di 1 il conteggio delle valide se temp è valida
                        else tempHead->valid = false;
                    }
                }
                printf("%d\n", x);    // stampa il numero di valide

                // una parola ammissibile letta e confrontata
                n--;
            }
            // esegue se la parola non è ammissibile
            else printf("not_exists\n");
        }
    }

    return 0;
}
