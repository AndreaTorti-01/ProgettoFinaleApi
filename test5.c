#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0
#define MAGIC_NUMBER 7919
#define MAXWORDLEN 128

uint32_t TABLESIZE = 0; // uint32_t max 4294967296
int k;
char buffer[MAXWORDLEN];
FILE *fileptr;
FILE *wfileptr;
bool ordered;

typedef struct node {
    char *word;
    bool valid;
    struct node *next;
} elem;
typedef elem *elem_ptr;

typedef struct chars {
    bool* bannedInPos;
    int minOcc;
    int Occ;
} chars_table;

void custom_strcpy(char* dest, char* source) {
    memcpy(dest, source, sizeof(char) * (k+1));
}

uint8_t map(char c) {
    if (c == '-') return (c - 45);
    else if (c >= '0' && c <= '9') return (c - 47);
    else if (c >= 'A' && c <= 'Z') return (c - 53);
    else if (c >= 'a' && c <= 'z') return (c - 59);
    else return (c - 48);
}

uint32_t multHash(char *key) {
    uint32_t hash = 0;
    int i;
    for (i = 0; i < k; ++i){
        hash = MAGIC_NUMBER * hash + key[i];
    }
    return hash % TABLESIZE;
}

elem_ptr head_insert(elem_ptr head, char *wordInput, bool validInput) {
    elem_ptr temp;
    temp = (elem_ptr)malloc(sizeof(elem));
    if (temp != NULL) {
        temp->next = head;
        temp->word = (char *)malloc(sizeof(char) * (k + 1));
        custom_strcpy(temp->word, wordInput);
        temp->valid = validInput;
        head = temp;
    }
    else {
        printf("\nErrore di allocazione.");
    }
    return head;
}

elem_ptr head_insert_check(elem_ptr head, char *wordInput, char *guessedChars, chars_table vincoli[], uint32_t *x) {
    int i, counts[64];
    elem_ptr temp;
    temp = (elem_ptr)malloc(sizeof(elem));
    for (i = 0; i < 64; i++)
        counts[i] = 0; // inizializzo i conteggi

    
    temp->next = head;
    temp->word = (char *)malloc(sizeof(char) * (k + 1));
    custom_strcpy(temp->word, wordInput);

    temp->valid = true;
    for (i = 0; i < k && temp->valid == true; i++) // scorre le lettere della parola da inserire
    {
        if (guessedChars[i] != '?') // se avevamo indovinato una lettera...
        { 
            if (wordInput[i] != guessedChars[i]) // e non è quella
                temp->valid = false; // invalidiamo
            else counts[map(wordInput[i])]++; // se era quella aumento di uno il numero di volte che l'ho trovata
        }
    }
    for (i = 0; i < k && temp->valid == true; i++) // le scorriamo di nuovo per contarle
    {
        if (vincoli[map(wordInput[i])].bannedInPos[i] == true) // se il char è bannato in quella posizione
            temp->valid = false; // invalidiamo
        else if (vincoli[map(wordInput[i])].minOcc != 0) // se ho già visto il char
            counts[map(wordInput[i])]++; // aumento il suo conteggio
    }
    for (i = 0; i < 64 && temp->valid == true; i++) // stavolta scorriamo tutti i char per controllare i conteggi
    {
        if (vincoli[i].minOcc != 0) // se c'è un numero minimo di volte che deve comparire
        {
            if (vincoli[i].Occ != 0) // o peggio un numero esatto
            {
                if (vincoli[i].Occ != counts[i]) // e non è quello
                    temp->valid = false; // invalido
            }
            else if (vincoli[i].minOcc > counts[i]) // e non compare almeno quel numero di volte
                temp->valid = false; // invalido
        }
    }

    if (temp->valid == true) // se la parola è (ancora) valida
        (*x)++; // aumento il conteggio di 1
    head = temp;
    return head;
}

elem_ptr remove_elem(elem_ptr head, char *wordInput) {
    elem_ptr temp;
    if (head == NULL) {
        return head;
    }
    if (strcmp(head->word, wordInput) == 0) {
        temp = head;
        head = head->next;
        free(temp->word);
        free(temp);
        return head;
    }
    head->next = remove_elem(head->next, wordInput);
    return head;
}

bool elem_in_list(elem_ptr head, char *wordInput) {
    for (; head != NULL; head = head->next)
    {
        if (strcmp(head->word, wordInput) == 0)
            return true;
    }
    return false;
}

void visualizzaLista(elem_ptr head) {
    for (; head != NULL; head = head->next)
        printf("%s(%d) -> ", head->word, head->valid);
    printf("NULL\n");
}

elem_ptr* rehash_and_double(elem_ptr* list){
    uint32_t i, hash;
    elem_ptr tempHead;

    TABLESIZE *= 2; // reimposta la dimensione dell'hashtable
    list = (elem_ptr *)realloc(list, sizeof(elem_ptr) * TABLESIZE); // espande l'hashtable
    for (i = TABLESIZE / 2; i < TABLESIZE; i++) // resetta i nuovi blocchi
        list[i] = NULL;
    for (i = 0; i < TABLESIZE / 2; i++){ // scorre i vecchi blocchi
        for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next){ // scorre all'interno dei blocchi
            hash = multHash(tempHead->word);
            if (hash != i){ // se la posizione non è più corretta...
                list[hash] = head_insert(list[hash], tempHead->word, tempHead->valid); // inserisco in coda in posizione hash
                list[i] = remove_elem(list[i], tempHead->word); // rimuovo l'elemento da posizione i
            }
        }
    }
    return list;
}

char** insert_in_array(char** array, char *word, uint32_t pos){
    array = (char**)realloc(array, sizeof(char*) * (pos + 1));
    array[pos] = (char*)malloc(sizeof(char) * (k + 1));
    custom_strcpy(array[pos], word);
    return array;
}

bool readline() {
    if (fgets(buffer, MAXWORDLEN, fileptr))
    {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // pulisce il buffer dal newline
        return true;
    }
    else
        return false;
}

bool validateSample(char *sample, char *word, char *guesses) {
    int i, j;
    bool isValid, found;
    isValid = true;
    for (i = 0; i < k && isValid == true; i++) // scorre le lettere della word e dei guesses
    {
        if (guesses[i] == '+') // se la lettera era indovinata...
        {
            if (sample[i] == word[i]) // ...ed è uguale a quella del sample
                sample[i] = '?'; // la usiamo e proseguiamo
            else
                isValid = false; // altrimenti invalidiamo subito
        }
        else if (guesses[i] == '|') // se invece era al posto sbagliato...
        {
            if (sample[i] == word[i])
                isValid = false;
            found = false; // la cerco
            for (j = 0; j < k && found == false; j++)
                if (sample[j] == word[i] && word[j] != word[i]) // trovata! ma non deve essere stata trovata dove non era permessa
                {
                    found = true;
                    sample[j] = '?'; // la usiamo
                }
            if (found == false)
                isValid = false;
        }
        else // se la lettera era assente dal riferimento...
            for (j = 0; j < k && isValid == true; j++) // ...la cerco
                if (sample[j] == word[i] && (word[j] != word[i] || guesses[j] != '+')) // trovata! ma non deve essere stata trovata sarà già al posto giusto
                    isValid = false; // trovata! invalido tutto
    }
    return isValid;
}

void merge(char** words, int low, int middle, int high) { // funzione dallo pseudocodice, leggermente modificata
    int i, j, q;
    int n1 = middle - low + 1;
    int n2 = high - middle;
    char left[n1][k + 1], right[n2][k + 1];
    for (i = 0; i < n1; i++)
        custom_strcpy(left[i], words[low + i]);
    for (j = 0; j < n2; j++)
        custom_strcpy(right[j], words[middle + 1 + j]);
    i = 0;
    j = 0;
    q = low;
    while (i < n1 && j < n2)
    {
        if (strcmp(left[i], right[j]) <= 0)
        {
            custom_strcpy(words[q], left[i]);
            i++;
        }
        else
        {
            custom_strcpy(words[q], right[j]);
            j++;
        }
        q++;
    }
    while (i < n1)
    {
        custom_strcpy(words[q], left[i]);
        i++;
        q++;
    }
    while (j < n2)
    {
        custom_strcpy(words[q], right[j]);
        j++;
        q++;
    }
}

void mergeSort(char** words, int low, int high) { // funzione dallo pseudocodice, leggermente modificata
    if (low < high) {
        int middle = (low + high) / 2;
        mergeSort(words, low, middle);
        mergeSort(words, middle + 1, high);
        merge(words, low, middle, high);
    }
}

void stampa_filtrate(elem_ptr *list, char **array, uint32_t x, uint32_t totalWords) {
    uint32_t i, xRead;
    elem_ptr tempHead;

    if (ordered == false) // ordina l'array se non è ordinato
        mergeSort(array, 0, totalWords - 1); // wtf
    ordered = true; // ora è ordinato!

    for (i = 0, xRead = 0; i < totalWords && xRead < x; i++){ // scorre le parole nell'array finchè non le finisco oppure ho letto tutte quelle valide
        for (tempHead = list[multHash(array[i])]; tempHead != NULL; tempHead = tempHead->next) // cerca se la parola è valida nella lista
            if ((strcmp(tempHead->word, array[i]) == 0) && (tempHead->valid == true)) { // se è valida
                fprintf(wfileptr, "%s\n", array[i]); // va stampata
                xRead++; // una in più letta
            }

    }
}

int main() {
    time_t t = clock();

    elem_ptr *list;
    char **array = NULL;
    elem_ptr tempHead;
    chars_table vincoli[64];
    int n; // n numero di turni ancora disponibili
    uint32_t hash, i, j, x, totalWords; // x numero parole valide, totalWords numero parole totali
    bool exit, found;
    fileptr = fopen("opentestcases/test3.txt", "r");
    wfileptr = fopen("opentestcases/test3.myoutput.txt", "w");

    totalWords = 0; // questo blocco conta le parole totali iniziali e imposta tablesize
    do {
        totalWords++;
        readline();
    } while (buffer[0] != '+');
    totalWords--;
    for (TABLESIZE = 1; TABLESIZE <= totalWords; TABLESIZE *= 2);
    TABLESIZE *= 4;
    rewind(fileptr);

    list = (elem_ptr *)calloc(TABLESIZE, sizeof(elem_ptr)); // inizializza l'hashtable

    readline();
    k = (int)strtol(buffer, (char **)NULL, 10); // imposta k

    char riferimento[k + 1], temp[k + 1], output[k + 1], guessedChars[k + 1]; // crea vari array di supporto

    x = 0; // popola l'hashtable e l'array di parole ammissibili
    exit = false;
    while (exit == false)
    {
        readline();
        if (buffer[0] != '+')
        {
            hash = multHash(buffer);
            list[hash] = head_insert(list[hash], buffer, true);
            array = insert_in_array(array, buffer, x);
            x++;
        }
        else
            exit = true;
    }
    ordered = false; // ora l'array è disordinato

    // inizia la partita
    for (i = 0; i < k; i++) // azzera guessedChars
        guessedChars[i] = '?';
    for (i = 0; i < 64; i++) // azzera i vincoli
    {
    vincoli[i].bannedInPos = (bool *)malloc(sizeof(bool) * k); // inizializza il vincolo bannedInPos
        for (j = 0; j < k; j++)
            vincoli[i].bannedInPos[j] = false;
        vincoli[i].minOcc = 0;
        vincoli[i].Occ = 0;
    }

    readline();
    custom_strcpy(riferimento, buffer); // copia la parola di riferimento

    readline();
    n = (int)strtol(buffer, (char **)NULL, 10); // legge il numero massimo di parole da confrontare

    while (readline()) // in buffer è contenuta la linea letta
    {
        if (buffer[0] == '+')
        {
            // stampa le parole ammissibili valide in ordine
            if (strcmp(buffer, "+stampa_filtrate") == 0)
                stampa_filtrate(list, array, x, totalWords);

            // popola ulteriormente la lista di parole ammissibili
            else if (strcmp(buffer, "+inserisci_inizio") == 0)
            {
                exit = false;
                while (exit == false) {
                    readline();
                    if (buffer[0] != '+') { // aggiunge parola all'hashtable e all'array
                        hash = multHash(buffer);
                        list[hash] = head_insert_check(list[hash], buffer, guessedChars, vincoli, &x);
                        array = insert_in_array(array, buffer, totalWords);
                        totalWords++;
                        if (totalWords * 2 > TABLESIZE)
                            list = rehash_and_double(list);
                    }
                    else
                        exit = true;
                }
                ordered = false; // ora l'array è disordinato
            }

            // inizia una nuova partita
            else if (strcmp(buffer, "+nuova_partita") == 0)
            {
                // rivalida tutte le parole e reiposta il conteggio delle valide al massimo
                x = 0;
                for (i = 0; i < TABLESIZE; i++)
                {
                    for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next)
                    {
                        tempHead->valid = true;
                        x++;
                    }
                }
                for (i = 0; i < k; i++) // azzera guessedChars
                    guessedChars[i] = '?';
                for (i = 0; i < 64; i++) // azzera i vincoli
                {
                    for (j = 0; j < k; j++)
                        vincoli[i].bannedInPos[j] = false;
                    vincoli[i].minOcc = 0;
                    vincoli[i].Occ = 0;
                }

                readline();
                custom_strcpy(riferimento, buffer); // copia la parola di riferimento

                readline();
                n = (int)strtol(buffer, (char **)NULL, 10); // legge il numero massimo di parole da confrontare
            }
        }

        // esegue se la parola è proprio r e può leggere parole: stampa ok e termina la partita
        else if (n > 0 && strcmp(buffer, riferimento) == 0)
        {
            fprintf(wfileptr, "ok\n");
            n = 0;
        }

        // esegue solo se può ancora leggere parole (e la parola non era r)
        else if (n > 0)
        {
            // esegue solo se la parola è ammissibile e la confronta con r: + ok, | ok wrong pos, / no.
            hash = multHash(buffer);
            if (elem_in_list(list[hash], buffer))
            {
                custom_strcpy(temp, riferimento); // mette la parola di riferimento in temp
                for (i = 0; i < k; i++) // scorro per cercare lettere indovinate
                {
                    output[i] = '/'; // imposto tutto l'output a "non esiste"
                    if (temp[i] == buffer[i]) // ho indovinato una lettera
                    {
                        output[i] = '+';
                        temp[i] = '?';
                        guessedChars[i] = buffer[i];
                        vincoli[map(buffer[i])].minOcc++; // aumento di uno il numero di occorrenze minime di tale lettera
                    }
                }
                for (i = 0; i < k; i++) // scorro per cercare lettere in posizione sbagliata
                {
                    if (output[i] != '+')
                    {
                        found = false;
                        for (j = 0; j < k && found == false; j++)
                        {
                            if (buffer[i] == temp[j]) // trovata
                            {
                                found = true;
                                temp[j] = '?';
                                output[i] = '|';
                                vincoli[map(buffer[i])].minOcc++; // aumento di uno il numero di occorrenze minime di tale lettera
                                vincoli[map(buffer[i])].bannedInPos[i] = true; // banno il char da quella posizione
                            }
                        }
                        // e se non l'ho trovata...
                        if (found == false && vincoli[map(buffer[i])].minOcc == 0) // potrei non averla mai trovata
                            for (j = 0; j < k; j++)
                                vincoli[map(buffer[i])].bannedInPos[j] = true; // la banno ovunque
                        else if (found == false && vincoli[map(buffer[i])].minOcc != 0) // o magari l'ho trovata in precedenza
                            vincoli[map(buffer[i])].Occ = vincoli[map(buffer[i])].minOcc; // allora adesso conosco il numero di occorrenze esatto!
                    }
                }
                output[k] = '\0';
                fprintf(wfileptr, "%s\n", output);

                // inizia la validazione di quelle ancora valide
                x = 0;
                for (i = 0; i < TABLESIZE; i++)
                {
                    for (tempHead = list[i]; tempHead != NULL; tempHead = tempHead->next)
                    {
                        if (tempHead->valid == true) // se è valida
                        {
                            custom_strcpy(temp, tempHead->word); // la mette in temp (sarà modificata!)
                            if (validateSample(temp, buffer, output) == true)
                                x++; // aumenta di 1 il conteggio delle valide se temp è valida
                            else
                                tempHead->valid = false;
                        }
                    }
                }
                fprintf(wfileptr, "%d\n", x); // stampa il numero di valide
                n--; // ho giocato un turno
                if (n == 0) fprintf(wfileptr, "ko\n"); // se non ho più turni a disposizione stampo ko
            }

            // esegue se la parola non è ammissibile
            else
                fprintf(wfileptr, "not_exists\n");
        }
    }

    printf("program took %f seconds to execute \n", ((double)t/CLOCKS_PER_SEC));

    return 0;
}
