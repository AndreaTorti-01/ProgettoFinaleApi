// lunghezza parole max: 8, numero parole e turni max: 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define bool	_Bool
#define true	(uint8_t)1
#define false	(uint8_t)0
#define MAXWORDLEN 256
#define wordlen_t uint8_t
#define nwords_t uint32_t

struct node{
    struct node* right;
    struct node* down;
    char *chunk;
    bool valid;
};
typedef struct node* node_ptr;

struct chars{
    bool* bannedInPos;
    wordlen_t minOcc;
    wordlen_t Occ;
};

wordlen_t k; // lunghezza parole
nwords_t x; // numero parole valide
nwords_t totalWords; // numero parole totali
nwords_t n; // numero turni a disposizione
struct chars constraints[64]; // tabella dei vincoli per ogni carattere
char buffer[MAXWORDLEN];
FILE *fileptr;
FILE *wfileptr;
char *guessedChars, *riferimento;

wordlen_t map(char c){
    if (c == '-') return (c - 45);
    else if (c >= '0' && c <= '9') return (c - 47);
    else if (c >= 'A' && c <= 'Z') return (c - 54);
    else if (c == '_') return (c - 58);
    else return (c - 59);
}

bool readline(){
    wordlen_t i = 0;
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

void cut_end(char* word, wordlen_t end){
    for (;end < strlen(word); end++)
        word[end] = '\0';
}

void slice(char *str, char *result, size_t start, size_t end){
    strncpy(result, str + start, end - start);
}

// ritorna la lunghezza del pezzo iniziale che corrisponde, 0 se non corrisponde, -1 se corrisponde fino alla fine di trieChunk
int8_t match_until(char* trieChunk, char* inputWord){
    int8_t num;
    int8_t max = strlen(trieChunk);
    for(num = 0; num < max && inputWord[num] == trieChunk[num]; num++);
    if (num == max) return -1;
    return num;
}

node_ptr trie_insert(char* word, node_ptr root){
    node_ptr prev = NULL;
    node_ptr temp = root;
    int8_t nMatch;

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

void print_trie(char* passed, wordlen_t index, node_ptr root){
    if (root->down == NULL){ // se sono in una foglia
        if (root->valid == true) fprintf(wfileptr, "%s%s\n", passed, root->chunk);
        if (root->right != NULL) print_trie(passed, index, root->right);
    }
    else{ // se sono in mezzo
        if (root->valid == true){
            char temp[strlen(passed) + 1];
            cut_end(passed, index);
            strcpy(temp, passed);
            strcat(temp, root->chunk);
            index += strlen(root->chunk);
            print_trie(temp, index, root->down);
        }
        if (root->right != NULL) print_trie(passed, index, root->right);
    }
}

bool is_in_trie(char* word, node_ptr root){
    int8_t nMatch;
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

// gioca la parola p, con riferimento r, stampando il risultato
void play_round(char* r, char* p){
    char output[k + 1];
    wordlen_t i, j, occs[64];
    for (i=0; i<64; i++) occs[i] = 0;
    bool found;
    for (i = 0; i < k; i++){ // scorro per cercare lettere indovinate
        output[i] = '/'; // imposto tutto l'output a "non esiste"
        if (p[i] == r[i]){ // ho indovinato una lettera
            output[i] = '+';
            r[i] = '?'; // la uso
            guessedChars[i] = p[i];
            occs[map(p[i])]++; // aumento di uno il numero di occorrenze trovate di tale lettera
        }
    }
    for (i = 0; i < k; i++){ // scorro per cercare lettere in posizione sbagliata
        if (output[i] != '+'){
            constraints[map(p[i])].bannedInPos[i] = true; // la banno da quella posizione
            found = false;

            // cerco la lettera nella parola di riferimento
            for (j = 0; j < k && found == false; j++){
                if (p[i] == r[j]){ // trovata
                    found = true;
                    output[i] = '|';
                    r[j] = '?'; // la uso
                    occs[map(p[i])]++; // aumento di uno il numero di occorrenze trovate di tale lettera
                }
            }

            if (found == false){ // e se non l'ho trovata...
                if (occs[map(p[i])] == 0) // potrei non averla mai trovata
                    for (j = 0; j < k; j++) constraints[map(p[i])].bannedInPos[j] = true; // la banno ovunque
                else // o magari l'ho trovata in precedenza
                    constraints[map(p[i])].Occ = occs[map(p[i])]; // allora adesso conosco il numero di occorrenze esatto!
            }
        }
    }
    for (i=0; i<64; i++) if (occs[i] > constraints[i].minOcc) constraints[i].minOcc = occs[i];
    output[k] = '\0';
    fprintf(wfileptr, "%s\n", output);
}

void validate_trie(node_ptr root, wordlen_t startIndex, wordlen_t counts[]){
    wordlen_t temp[64];
    wordlen_t tempIndex = startIndex;
    memcpy(temp, counts, sizeof(wordlen_t) * 64);

    if (root->valid == true){ // se il nodo è ancora valido...
        wordlen_t i, len = strlen(root->chunk);
        for (i=0; i<len && root->valid; i++){ // cerco di invalidare il generico nodo
            counts[map(root->chunk[i])]++;
            if ((guessedChars[i+startIndex] != '?' && root->chunk[i] != guessedChars[i+startIndex]) || // se il char è in una posizione indovinata e non è quello corretto
                (constraints[map(root->chunk[i])].bannedInPos[i+startIndex] == true) || // se il char è bannato da quella posizione
                (constraints[map(root->chunk[i])].Occ != 0 && counts[map(root->chunk[i])] > constraints[map(root->chunk[i])].Occ)){ // se ho superato il numero di occorrenze esatto
                root->valid = false;
            }
        }
        if (root->valid){ // se è ancora valido
            if (root->down == NULL){ // se sono in una foglia
                for (i=0; i<64 && root->valid; i++){ // per ogni lettera
                    if ((constraints[i].Occ != 0 && counts[i] != constraints[i].Occ) || // se non ho il numero esatto di occorrenze
                        (constraints[i].minOcc != 0 && counts[i] < constraints[i].minOcc)){ // oppure il numero minimo
                        root->valid = false;
                    }
                }
                if (root->valid) x++; // se sono arrivato fin qui ho trovato una parola valida
            }
            else{ // se sono in mezzo
                validate_trie(root->down, startIndex + len, counts); // inizio a validare di sotto
            }
        }
    }

    if (root->right != NULL){ // sicuramente vanno validate le parole a destra
        validate_trie(root->right, tempIndex, temp);
    }
}

void revalidate_trie(node_ptr root){
    root->valid = true;
    if (root->right != NULL) revalidate_trie(root->right);
    if (root->down != NULL) revalidate_trie(root->down);
}

void newGameReset (node_ptr root){
    wordlen_t i, j;

    // reimposta x al numero di parole
    x = totalWords;

    // rivalida tutto il dizionario
    revalidate_trie(root);

    // azzera guessedChars e i vincoli
    for (i = 0; i < k; i++)
        guessedChars[i] = '?';
    for (i = 0; i < 64; i++) {
        for (j = 0; j < k; j++)
            constraints[i].bannedInPos[j] = false;
        constraints[i].minOcc = 0;
        constraints[i].Occ = 0;
    }

    // imposta la parola di riferimento
    readline();
    strcpy(riferimento, buffer); 

    // imposta n
    readline();
    n = (nwords_t)strtol(buffer, (char **)NULL, 10);
}

int main(){
    node_ptr root = NULL; // radice del trie
    wordlen_t counts[64]; // numero occorrenze lettera temporaneo (usato in validate_trie)

    bool exit; // variabili di supporto
    wordlen_t i; // variabili di supporto

    fileptr = fopen("tests/input_HS014.txt", "r");
    wfileptr = fopen("tests/input_HS014.myoutput.txt", "w");

    // imposta k
    readline();
    k = (wordlen_t)strtol(buffer, (char **)NULL, 10);

    // crea vari array di supporto e alloca gli spazi necessari scoperti
    char temp[k + 1];
    guessedChars = malloc(sizeof(char) * (k + 1));
    riferimento = malloc(sizeof(char) * (k + 1));

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
    newGameReset(root);

    while (readline()){ // in buffer è contenuta la linea letta

        // se era un comando
        if (buffer[0] == '+'){

            // stampa le parole ammissibili valide in ordine
            if (strcmp(buffer, "+stampa_filtrate") == 0){
                for(i=0; i<k+1; i++) temp[i] = '\0';
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
                for (i=0; i<64; i++) counts[i] = 0;
                x = 0;
                validate_trie(root, 0, counts);
            }

            // inizia una nuova partita
            else if (strcmp(buffer, "+nuova_partita") == 0)
                newGameReset(root);
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
                play_round(temp, buffer);
                for (i=0; i<64; i++) counts[i] = 0;
                x = 0;
                validate_trie(root, 0, counts);
                fprintf(wfileptr, "%d\n", x);
                n--; // ho giocato un turno
                if (n == 0) fprintf(wfileptr, "ko\n"); // se non ho più turni a disposizione stampo ko
            }

            // se la parola non è ammissibile
            else fprintf(wfileptr, "not_exists\n");
        }
    }

    return 0;
}
