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

typedef struct chars{
    bool banned;
    int maxOcc;
} chars_table;

int map(char c){
    if (c == '-') return (c-45);
    else if (c >= '0' && c <= '9') return (c-47);
    else if (c >= 'A' && c <= 'Z') return (c-53);
    else if (c >= 'a' && c <= 'z') return (c-59);
    else return (c-48);
}

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

elem_ptr head_insert_check(elem_ptr head, char* wordInput, char* guessedChars, chars_table vincoli[]){
    int i, j, counts[64];
    elem_ptr temp;
    temp = (elem_ptr) malloc(sizeof(elem));
    for (i=0; i<64; i++) counts[i] = 0; // inizializzo i conteggi
    if (temp != NULL){
        temp->next = head;
        temp->word = (char*) malloc(sizeof(char) * (k + 1));
        strncpy(temp->word, wordInput, k+1);
        temp->valid = true;
        for (i=0; i<k && temp->valid == true; i++){ // scorre le lettere della parola da inserire
            if (guessedChars[i] != '?'){    // se avevamo indovinato una lettera...
                if (wordInput[i] != guessedChars[i]) temp->valid = false;   // e non è quella, invalidiamo
            }
        }
        for (i=0; i<k && temp->valid == true; i++){ // le scorriamo di nuovo
            if (vincoli[map(wordInput[i])].banned == true) temp->valid = false; // se il char è bannato, invalidiamo
            else if (counts[map(wordInput[i])] < vincoli[map(wordInput[i])].maxOcc) counts[map(wordInput[i])]++;    // se ho ancora counts disponibili,aumento il count di quel char
            else temp->valid = false;   // se non ne ho più disponibili invalido
        }
        head = temp;
    } else printf("\nErrore di allocazione.");
    return head;
}

elem_ptr tail_insert_check(elem_ptr head, char* wordInput, char* guessedChars, chars_table vincoli[]){   /*per creare una lista da zero è importante inizializzare la testa a null*/ 
    if (head == NULL) return head_insert_check(head, wordInput, guessedChars, vincoli);
    head->next = tail_insert_check(head->next, wordInput, guessedChars, vincoli);
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

void merge (char words[][k+1], int low, int middle, int high) {
    int i, j, q;
    int n1 = middle - low + 1;
    int n2 = high - middle;
    char left[n1][k+1], right[n2][k+1];
    for (i = 0; i < n1; i++)
        strncpy(left[i], words[low + i], k+1);
    for (j = 0; j < n2; j++)
        strncpy(right[j], words[middle + 1 + j], k+1);
    i = 0;
    j = 0;
    q = low;
    while (i < n1 && j < n2) {
        if (strcmp(left[i], right[j]) <= 0) {
            strncpy(words[q], left[i], k+1);
            i++;
        } else {
            strncpy(words[q], right[j], k+1);
            j++;
        }
        q++;
    }
    while (i < n1) {
        strncpy(words[q], left[i], k+1);
        i++;
        q++;
    }
    while (j < n2) {
        strncpy(words[q], right[j], k+1);
        j++;
        q++;
    }
}

void mergeSort (char words[][k+1], int low, int high) {
    if (low < high) {
        int middle = (low + high) / 2;
        mergeSort(words, low, middle);
        mergeSort(words, middle + 1, high);
        merge(words, low, middle, high);
    }
}

void stampa_filtrate(elem_ptr* list, int x){
    char words[x][k+1];
    int i, xTmp;
    elem_ptr tempHead;
    xTmp = 0;
    for (i=0; i<TABLESIZE; i++){    // scorre linee della tabella
        for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next){   // scorre elementi della linea
            if (tempHead->valid){
                strncpy(words[xTmp], tempHead->word, k+1);
                xTmp++;
            }
        }
    }
    if (x != 1) mergeSort(words, 0, x);
    for (xTmp=0; xTmp<x; xTmp++) printf("%s\n", words[xTmp]);
}

int main(){
    elem_ptr* list;
    elem_ptr tempHead;
    chars_table vincoli[64];
    int hash, i, j, q, n, x;    // n numero di turni ancora disp, x numero parole valide
    bool exit, found;
    fileptr = fopen("opentestcases/test3.txt", "r");

    list = (elem_ptr*) malloc(sizeof(elem_ptr) * TABLESIZE);    // inizializza la hashtable
    for (i=0; i<TABLESIZE; i++)
        list[i] = NULL;

    readline();
    k = (int)strtol(buffer, (char **)NULL, 10);  // imposta k

    char riferimento[k+1], temp[k+1], output[k+1], guessedChars[k+1];   // crea vari array di supporto

    for (i=0; i<k; i++) guessedChars[i] = '?';  // inizializza guessedChars
    for (i=0; i<64; i++){   // inizializza le regole
        vincoli[i].banned = false;
        vincoli[i].maxOcc = 0;
    }

    // popola la hashtable di parole ammissibili
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
                stampa_filtrate(list, x);
            }

            // esegue e popola ulteriormente la lista di parole ammissibili
            else if (strcmp(buffer, "+inserisci_inizio") == 0){
                exit = false;
                while (exit == false){
                    readline();
                    if (buffer[0] != '+'){
                        hash = MultHash(buffer);
                        list[hash] = tail_insert_check(list[hash], buffer, guessedChars, vincoli);
                    } else exit = true;
                }
            }

            else if (strcmp(buffer, "+nuova_partita") == 0){
                // rivalida tutte le parole
                for (i=0; i<TABLESIZE; i++){    // scorre linee della tabella
                    for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next){   // scorre elementi della linea
                        tempHead->valid = true;
                    }
                }

                for (i=0; i<64; i++){   // inizializza i vincoli
                    vincoli[i].banned = false;
                    vincoli[i].maxOcc = 0;
                }
                for (i=0; i<k; i++) guessedChars[i] = '?';  // inizializza guessedChars

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
                strncpy(temp, riferimento, k+1);    // mette la parola di riferimento in temp
                for (i=0; i<k; i++){
                    output[i] = '/';
                    if (temp[i] == buffer[i]){
                        output[i] = '+';
                        temp[i] = '?';
                        guessedChars[i] = buffer[i];
                    }
                    else{
                        for (j=0; j<k; j++){
                            if (buffer[i] == temp[j]){
                                temp[j] = '?';
                                output[i] = '|';
                                vincoli[map(buffer[i])].maxOcc++;
                            }
                        }
                        // se non ha mai trovato buffer[i] lo banna
                        if (output[i] == '/' && vincoli[map(buffer[i])].maxOcc == 0) vincoli[map(buffer[i])].banned = true;
                    }
                }
                output[k] = '\0';
                printf("%s\n", output);
                // inizia la validazione di quelle ancora valide
                x = 0;
                for (i=0; i<TABLESIZE; i++){    // scorre linee della tabella
                    for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next){   // scorre elementi della linea
                        if (tempHead->valid){
                            strncpy(temp, tempHead->word, k+1);   // mette l'elemento in temp (sarà modificato!)
                            if (validateSample(temp, buffer, output)){
                                x++;
                            }   // aumenta di 1 il conteggio delle valide se temp è valida
                            else tempHead->valid = false;
                        }
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
