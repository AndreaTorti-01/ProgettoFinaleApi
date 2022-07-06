// dynamic array of strings, with radix sort only when useful

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0
#define MAGIC_NUMBER 7919
#define MAXWORDLEN 128

uint32_t TABLESIZE = 0; // uint32_t max 4294967296
int k;
bool ordered;
char buffer[MAXWORDLEN];
FILE *fileptr;
FILE *wfileptr;

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

void scambia_str(char **a, char **b) {
    char *temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

/* chiama con k=0 e n numero di parole*/
void radixSort(int n, char **a, int k) {
    int i;
    uint32_t count[UCHAR_MAX+1];
    int mode;
    char **bin[UCHAR_MAX+1];
    char **lim[UCHAR_MAX+1];

    while(n > 1) {
        memset(count, 0, sizeof(int)*(UCHAR_MAX+1));
        for(i = 0; i < n; i++) {
            count[(unsigned char) a[i][k]]++;
        }
        mode = 1;
        for(i = 2; i < UCHAR_MAX+1; i++) {
            if(count[i] > count[mode]) {
                mode = i;
            }
        }
        if(count[mode] < n) {
            bin[0] = lim[0] = a;
            for(i = 1; i < UCHAR_MAX+1; i++) {
                lim[i] = bin[i] = bin[i-1] + count[i-1];
            }
            for(i = 0; i < UCHAR_MAX+1; i++) {
                while(lim[i] < bin[i] + count[i]) {
                    if((unsigned char) lim[i][0][k] == i) {
                        lim[i]++;
                    } else {
                        scambia_str(lim[i], lim[(unsigned char) lim[i][0][k]]++);
                    }
                }
            }
            for(i = 1; i < UCHAR_MAX+1; i++) {
                if(i != mode) {
                    radixSort(count[i], bin[i], k+1);
                }
            }
            n = count[mode];
            a = bin[mode];
        }
        k++;
    }
}

elem_ptr head_insert(elem_ptr head, char *wordInput, bool validInput) {
    elem_ptr temp;
    temp = (elem_ptr)malloc(sizeof(elem));
    if (temp != NULL) {
        temp->next = head;
        temp->word = (char *)malloc(sizeof(char) * (k + 1));
        strcpy(temp->word, wordInput);
        temp->valid = validInput;
        head = temp;
    }
    else {
        printf("\nErrore di allocazione.");
    }
    return head;
}

// aggiunge 1 a x se wordInput è valido rispetto ai vincoli attuali
elem_ptr head_insert_check(elem_ptr head, char *wordInput, char *guessedChars, chars_table vincoli[], uint32_t *x) {
    int i, counts[64];
    elem_ptr temp;
    temp = (elem_ptr)malloc(sizeof(elem));
    for (i = 0; i < 64; i++)
        counts[i] = 0; // inizializzo i conteggi

    
    temp->next = head;
    temp->word = (char *)malloc(sizeof(char) * (k + 1));
    strcpy(temp->word, wordInput);

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

char **append_word(char **array, char* word, uint32_t position){
    array = (char**)realloc(array, sizeof(char*) * (position+1));
    array[position] = calloc(k+1, sizeof(char));
    strcpy(array[position], word);
    return array;
}

bool word_is_valid(elem_ptr head, char *wordInput) {
    for (; head != NULL; head = head->next)
    {
        if (strcmp(head->word, wordInput) == 0 && head->valid)
            return true;
    }
    return false;
}

bool word_exists(elem_ptr head, char *wordInput) {
    for (; head != NULL; head = head->next)
    {
        if (strcmp(head->word, wordInput) == 0)
            return true;
    }
    return false;
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

void stampa_filtrate(elem_ptr *list,char **array, uint32_t x, uint32_t totalWords) {
    uint32_t xRead, i;
    if (ordered == false){
        radixSort(totalWords, array, 0);
        ordered = true;
    }
    for (xRead = 0, i = 0; xRead < x; i++) {  // scorre le parole già ordinate
        if (word_is_valid(list[multHash(array[i])], array[i])){  // se è valida
            xRead++;
            fprintf(wfileptr, "%s\n", array[i]); // va stampata
        }
    }
}

int main() {
    elem_ptr *list = NULL;
    char** array = malloc(0); // array ordinato del dizionario
    elem_ptr tempHead;
    chars_table vincoli[64];
    int n; // n numero di turni ancora disponibili
    uint32_t hash, i, j, x, totalWords; // x numero parole valide, totalWords numero parole totali
    bool exit, found;
    fileptr = fopen("opentestcases/upto18_2.txt", "r");
    wfileptr = fopen("opentestcases/upto18_2.myoutput.txt", "w");

    totalWords = 0; // questo blocco conta le parole totali iniziali e imposta tablesize
    do {
        totalWords++;
        readline();
    } while (buffer[0] != '+');
    totalWords-=2;
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
            array = append_word(array, buffer, x);
            x++;
        }
        else
            exit = true;
    }
    ordered = false;

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
    strcpy(riferimento, buffer); // copia la parola di riferimento

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
                        array = append_word(array, buffer, totalWords);
                        totalWords++;
                        if (totalWords * 2 > TABLESIZE)
                            list = rehash_and_double(list);
                    }
                    else
                        exit = true;
                }
                ordered = false;
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
                strcpy(riferimento, buffer); // copia la parola di riferimento

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
            if (word_exists(list[hash], buffer))
            {
                strcpy(temp, riferimento); // mette la parola di riferimento in temp
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
                            strcpy(temp, tempHead->word); // la mette in temp (sarà modificata!)
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
    return 0;
}
