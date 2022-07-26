#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0
#define MAGIC_NUMBER 7919
#define MAXWORDLEN 32

int k;
char buffer[MAXWORDLEN];
FILE *fileptr;
FILE *wfileptr;

struct node{
    struct node* right;
    struct node* down;
    char *chunk;
    bool valid;
};
typedef struct node* node_ptr;

typedef struct chars {
    bool* bannedInPos;
    int minOcc;
    int Occ;
} chars_table;

uint8_t map(char c){
    if (c == '-') return (c - 45);
    else if (c >= '0' && c <= '9') return (c - 47);
    else if (c >= 'A' && c <= 'Z') return (c - 53);
    else if (c >= 'a' && c <= 'z') return (c - 59);
    else return (c - 48);
}

bool readline(){
    char ch;
    int i=0;
    buffer[i] = fgetc(fileptr);
    while (buffer[i] != EOF){
        i++;
        if ((buffer[i] = fgetc(fileptr)) == '\n'){
            buffer[i] = '\0';
            return true;
        }
    }
    return false;
}

void cut_end(char* word, int end){
    for (;end < strlen(word); end++)
        word[end] = '\0';
}

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
    node_ptr prev = NULL;
    node_ptr temp = root;
    int nMatch;

    while(1){
        if (temp!= NULL) nMatch = match_until(temp->chunk, word);
        else nMatch = 0;
        
        if (nMatch > 0){ // deve inserire in temp->down wordRemainder e trieRemainder, ovvero le parti che non corrispondono
            char wordRemainder[strlen(word) - nMatch + 1];
            char* trieRemainder = malloc(sizeof(char) * (strlen(temp->chunk) - nMatch + 1));
            slice(word, wordRemainder, nMatch, strlen(word)+1); // mette in wordRemainder i caratteri di word che non corrispondevano
            slice(temp->chunk, trieRemainder, nMatch, strlen(temp->chunk)+1); // mette in trieRemainder i caratteri di chunk che non corrispondevano

            temp->chunk = realloc(temp->chunk, sizeof(char) * (nMatch+1)); // rimpicciolisce la parola già presente nel trie
            temp->chunk[nMatch] = '\0';
            prev = temp->down;
            temp->down = malloc(sizeof(struct node));  // inserisce il pezzo rimasto sotto
            temp->down->down = prev;
            temp->down->valid = temp->valid; // eredita la validità dal pezzo sopra
            temp->valid = true; // e il pezzo sopra è valido
            temp->down->chunk = trieRemainder;
            temp->down = trie_insert(wordRemainder, temp->down); // ed ora inserisce il remainder della word nel livello sotto
            return root;
        }
        else if (nMatch == -1){ // se invece corrisponde tutto dobbiamo solo inserire il wordRemainder al livello sotto
            char wordRemainder[strlen(word) - strlen(temp->chunk) + 1];
            slice(word, wordRemainder, strlen(temp->chunk), strlen(word)+1);
            temp->down = trie_insert(wordRemainder, temp->down);
            return root;
            
        }
        else{ // se non riesce a inserire
            if (temp == NULL){ // magari è la fine della lista
                temp = malloc(sizeof(struct node));
                temp->right = temp->down = NULL;
                temp->valid = true;
                if (prev != NULL) prev->right = temp; // solo se non è il primo inserimento in assoluto
                temp->chunk = malloc(sizeof(char) * (strlen(word) + 1));
                strcpy(temp->chunk, word);
                if (prev == NULL) root = temp; // solo se è il primo inserimento in assoluto
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
        if (root->valid) fprintf(wfileptr, "%s%s\n", passed, root->chunk);
        if (root->right != NULL) 
            if (root->right->valid) print_trie(passed, index, root->right);
    }
    else{ // se sono in mezzo
        if (root->down->valid){
            char temp[strlen(passed) + 1];
            cut_end(passed, index);
            strcpy(temp, passed);
            strcat(temp, root->chunk);
            index += strlen(root->chunk);
            print_trie(temp, index, root->down);
        }
        if (root->right != NULL){
            if (root->right->valid) print_trie(passed, index, root->right);
        }
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

// gioca la parola di buffer, con riferimento temp, stampando il risultato
void play_round(char* output, char* temp, char* guessedChars, char* buffer, chars_table constraints[]){
    int i, j;
    bool found;
    for (i = 0; i < k; i++){ // scorro per cercare lettere indovinate
        output[i] = '/'; // imposto tutto l'output a "non esiste"
        if (temp[i] == buffer[i]){ // ho indovinato una lettera
            output[i] = '+';
            temp[i] = '?';
            guessedChars[i] = buffer[i];
            constraints[map(buffer[i])].minOcc++; // aumento di uno il numero di occorrenze minime di tale lettera
        }
    }
    for (i = 0; i < k; i++){ // scorro per cercare lettere in posizione sbagliata
        if (output[i] != '+'){
            found = false;
            for (j = 0; j < k && found == false; j++){
                if (buffer[i] == temp[j]){ // trovata
                    found = true;
                    temp[j] = '?';
                    output[i] = '|';
                    constraints[map(buffer[i])].minOcc++; // aumento di uno il numero di occorrenze minime di tale lettera
                    constraints[map(buffer[i])].bannedInPos[i] = true; // banno il char da quella posizione
                }
            }
            // e se non l'ho trovata...
            if (found == false && constraints[map(buffer[i])].minOcc == 0) // potrei non averla mai trovata
                for (j = 0; j < k; j++)
                    constraints[map(buffer[i])].bannedInPos[j] = true; // la banno ovunque
            else if (found == false && constraints[map(buffer[i])].minOcc != 0) // o magari l'ho trovata in precedenza
                constraints[map(buffer[i])].Occ = constraints[map(buffer[i])].minOcc; // allora adesso conosco il numero di occorrenze esatto!
        }
    }
    output[k] = '\0';
    fprintf(wfileptr, "%s\n", output);
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

void newGameReset (node_ptr root, uint32_t *x, uint32_t *totalWords, int *n, char *guessedChars, char *riferimento, chars_table constraints[]){
    // reimposta x al numero di parole
    (*x) = (*totalWords);

    // rivalida tutto il dizionario
    revalidate_trie(root);

    // imposta la parola di riferimento
    readline();
    strcpy(riferimento, buffer); 

    // imposta n
    readline();
    (*n) = (int)strtol(buffer, (char **)NULL, 10);
}

void validate_trie(node_ptr root, chars_table constraints[], char *guessedChars, uint32_t *x){

}

int main(){
    node_ptr root = NULL; // radice del trie
    chars_table constraints[64]; // tabella dei vincoli per ogni carattere
    bool exit; // variabili di supporto
    uint32_t i; // variabili di supporto
    uint32_t x; // numero parole valide
    uint32_t totalWords; // numero parole totali
    int n; // numero turni a disposizione
    fileptr = fopen("opentestcases/test3.txt", "r");
    wfileptr = fopen("opentestcases/test3.myoutput.txt", "w");

    // imposta k
    readline();
    k = (int)strtol(buffer, (char **)NULL, 10);

    // crea vari array di supporto
    char riferimento[k + 1], temp[k + 1], output[k + 1], guessedChars[k + 1];

    // crea il dizionario e imposta il numero di parole totali
    exit = false;
    totalWords = 0;
    while (!exit){
        readline();
        if (buffer[0] != '+'){
            root = trie_insert(buffer, root);
            totalWords++;
        }
        else if (strcmp(buffer, "+inserisci_inizio") != 0 && strcmp(buffer, "+inserisci_fine") != 0)
            exit = true;
    }

    // alloca lo spazio per il vincolo bannedinpos
    for (i = 0; i < 64; i++) constraints[i].bannedInPos = (bool *)malloc(sizeof(bool) * k);

    // inizia la prima partita
    newGameReset(root, &x, &totalWords, &n, guessedChars, riferimento, constraints);

    while (readline()){ // in buffer è contenuta la linea letta

        // se era un comando
        if (buffer[0] == '+'){

            // stampa le parole ammissibili valide in ordine
            if (strcmp(buffer, "+stampa_filtrate") == 0){
                for(i=0; i<k+1; i++) temp[k] = '\0';
                print_trie(temp, 0, root);
            }

            // popola ulteriormente la lista di parole ammissibili
            else if (strcmp(buffer, "+inserisci_inizio") == 0){
                exit = false;
                while (exit == false){
                    readline();
                    if (buffer[0] != '+'){
                        root = trie_insert(buffer, root);
                        totalWords++;
                    }
                    else
                        exit = true;
                }
                validate_trie(root, constraints, guessedChars, &x);
            }

            // inizia una nuova partita
            else if (strcmp(buffer, "+nuova_partita") == 0)
                newGameReset(root, &x, &totalWords, &n, guessedChars, riferimento, constraints);
        }

        // se la parola è proprio r e può leggere parole: stampa ok e termina la partita
        else if (n > 0 && strcmp(buffer, riferimento) == 0){
            fprintf(wfileptr, "ok\n");
            n = 0;
        }

        // se può ancora leggere parole (e la parola non era r)
        else if (n > 0){
            // se la parola è ammissibile gioca un round
            if (is_in_trie(buffer, root)){
                strcpy(temp, riferimento); // mette la parola di riferimento in temp
                play_round(output, temp, guessedChars, buffer, constraints);
                validate_trie(root, constraints, guessedChars, &x);
                n--; // ho giocato un turno
                if (n == 0) fprintf(wfileptr, "ko\n"); // se non ho più turni a disposizione stampo ko
            }

            // se la parola non è ammissibile
            else fprintf(wfileptr, "not_exists\n");
        }
    }

    return 0;
}