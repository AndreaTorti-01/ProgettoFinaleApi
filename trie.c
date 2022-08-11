// lunghezza parole max: 8, numero parole e turni max: 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXWORDLEN 128

struct node{
    struct node* right;
    struct node* down;
    char *chunk;
};
typedef struct node* node_ptr;

struct chars{
    bool* bannedInPos;
    uint8_t minOcc;
    uint8_t Occ;
};

uint8_t k; // lunghezza parole
uint32_t x; // numero parole valide
uint32_t totalWords; // numero parole totali
uint32_t n; // numero turni a disposizione
struct chars constraints[64]; // tabella dei vincoli per ogni carattere
char buffer[MAXWORDLEN];
char *guessedChars, *riferimento;

uint8_t map(char c){
    if (c == '-') return (c - 45);
    else if (c >= '0' && c <= '9') return (c - 47);
    else if (c >= 'A' && c <= 'Z') return (c - 54);
    else if (c == '_') return (c - 58);
    else return (c - 59);
}

bool readline(){
    if (fgets(buffer, MAXWORDLEN, stdin)) {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // pulisce il buffer dal newline
        return true;
    }
    else
        return false;
}

bool node_valid(node_ptr root){
    if (root->chunk[strlen(root->chunk) + 1] != '-') return true;
    return false;
}

void node_set(node_ptr root, bool valid){
    if (valid) root->chunk[strlen(root->chunk) + 1] = '+';
    else root->chunk[strlen(root->chunk) + 1] = '-';
}

void cut_end(char* word, uint8_t end){
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
        if (temp != NULL) nMatch = match_until(temp->chunk, word);
        else nMatch = 0;
        
        if (nMatch > 0){ // deve inserire in temp->down wordRemainder e trieRemainder, ovvero le parti che non corrispondono
            char wordRemainder[strlen(word) - nMatch + 1];
            char* trieRemainder = malloc(sizeof(char) * (strlen(temp->chunk) - nMatch + 2));
            slice(word, wordRemainder, nMatch, strlen(word) + 1); // mette in wordRemainder i caratteri di word che non corrispondevano
            slice(temp->chunk, trieRemainder, nMatch, strlen(temp->chunk) + 2); // mette in trieRemainder i caratteri di chunk che non corrispondevano

            bool tempValid = node_valid(temp);
            temp->chunk = realloc(temp->chunk, sizeof(char) * (nMatch + 2)); // rimpicciolisce il nodo già presente nel trie
            temp->chunk[nMatch] = '\0';
            node_set(temp, true); // e lo imposta come valido
            prev = temp->down;
            temp->down = malloc(sizeof(struct node));  // inserisce il pezzo rimasto sotto
            temp->down->down = prev;
            temp->down->chunk = trieRemainder;
            node_set(temp->down, tempValid); // eredita la validità dal pezzo sopra
            temp->down = trie_insert(wordRemainder, temp->down); // ed ora inserisce il remainder della word nel livello sotto
            return root;
        }
        else if (nMatch == -1){ // se invece corrisponde tutto dobbiamo solo inserire il wordRemainder al livello sotto
            char wordRemainder[strlen(word) - strlen(temp->chunk) + 1];
            slice(word, wordRemainder, strlen(temp->chunk), strlen(word) + 1);
            temp->down = trie_insert(wordRemainder, temp->down);
            return root;
            
        }
        else{ // se non riesce a inserire
            if (temp == NULL){ // magari è la fine della lista
                temp = malloc(sizeof(struct node));
                temp->right = temp->down = NULL;
                if (prev != NULL) prev->right = temp; // solo se non è il primo inserimento in assoluto
                temp->chunk = malloc(sizeof(char) * (strlen(word) + 2));
                strcpy(temp->chunk, word);
                node_set(temp, true);
                if (prev == NULL) root = temp; // solo se è il primo inserimento in assoluto
                return root;
            }
            if (temp->chunk[0] > word[0]){ // oppure sto inserendo prima della fine
                if (prev == NULL) { // magari all'inizio
                    prev = malloc(sizeof(struct node));
                    prev->right = temp;
                    prev->down = NULL;
                    prev->chunk = malloc(sizeof(char) * (strlen(word) + 2));
                    strcpy(prev->chunk, word);
                    node_set(prev, true);
                    return prev;
                }
                else{ // oppure in mezzo
                    temp = malloc(sizeof(struct node));
                    temp->right = prev->right;
                    prev->right = temp;
                    temp->down = NULL;
                    temp->chunk = malloc(sizeof(char) * (strlen(word) + 2));
                    strcpy(temp->chunk, word);
                    node_set(temp, true);
                    return root;
                }
            }
            prev = temp;
            temp = temp->right; // scorre a destra
        }
    }
}

void print_trie(char* passed, uint8_t index, node_ptr root){
    if (root->down == NULL){ // se sono in una foglia
        if (node_valid(root)) printf("%s%s\n", passed, root->chunk);
        if (root->right != NULL) print_trie(passed, index, root->right);
    }
    else{ // se sono in mezzo
        if (node_valid(root)){
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
    uint8_t i, j, occs[64];
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
    printf("%s\n", output);
}

void validate_trie(node_ptr root, uint8_t startIndex, uint8_t counts[]){
    uint8_t temp[64];
    uint8_t tempIndex = startIndex;
    memcpy(temp, counts, sizeof(uint8_t) * 64);

    if (node_valid(root)){ // se il nodo è ancora valido...
        uint8_t i, len = strlen(root->chunk);
        if (root->down == NULL){ // se sono in una foglia
            for (i=0; i<len && node_valid(root); i++){ // per ogni lettera
                counts[map(root->chunk[i])]++;
                if ( (guessedChars[i+startIndex] != '?' && root->chunk[i] != guessedChars[i+startIndex]) || // se il char è in una posizione indovinata e non è quello corretto
                        (constraints[map(root->chunk[i])].bannedInPos[i+startIndex] == true) ) { // se il char è bannato da quella posizione
                    node_set(root, false);
                }
            }
            for (i=0; i<64 && node_valid(root); i++){
                if( (constraints[i].Occ != 0 && counts[i] != constraints[i].Occ) || // se non ho il numero esatto di occorrenze
                    (constraints[i].minOcc != 0 && counts[i] < constraints[i].minOcc) ) { // oppure il numero minimo
                    node_set(root, false);
                }
            }
            if (node_valid(root)) x++; // se sono arrivato fin qui ho trovato una parola valida
        }
        else{ // se sono in mezzo
            for (i=0; i<len && node_valid(root); i++){ // cerco di invalidare il nodo
                counts[map(root->chunk[i])]++;
                if ((guessedChars[i+startIndex] != '?' && root->chunk[i] != guessedChars[i+startIndex]) || // se il char è in una posizione indovinata e non è quello corretto
                    (constraints[map(root->chunk[i])].bannedInPos[i+startIndex] == true) || // se il char è bannato da quella posizione
                    (constraints[map(root->chunk[i])].Occ != 0 && counts[map(root->chunk[i])] > constraints[map(root->chunk[i])].Occ)){ // se ho superato il numero di occorrenze esatto
                    node_set(root, false);
                }
            }
            if (node_valid(root)) validate_trie(root->down, startIndex + len, counts); // inizio a validare di sotto
        }
    }

    if (root->right != NULL){ // sicuramente vanno validate le parole a destra
        validate_trie(root->right, tempIndex, temp);
    }
}

void revalidate_trie(node_ptr root){
    node_set(root, true);
    if (root->right != NULL) revalidate_trie(root->right);
    if (root->down != NULL) revalidate_trie(root->down);
}

void newGameReset (node_ptr root){
    uint8_t i, j;

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
    n = (uint32_t)strtol(buffer, (char **)NULL, 10);
}

int main(){
    node_ptr root = NULL; // radice del trie
    uint8_t* counts = (uint8_t*)malloc(sizeof(uint8_t) * 64); // numero occorrenze lettera temporaneo (usato in validate_trie)

    bool exit; // variabili di supporto
    uint8_t i; // variabili di supporto

    // imposta k
    readline();
    k = (uint8_t)strtol(buffer, (char **)NULL, 10);

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
                memset(temp, '\0', k+1);
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
                memset(counts, 0, 64);
                x = 0;
                validate_trie(root, 0, counts);
            }

            // inizia una nuova partita
            else if (strcmp(buffer, "+nuova_partita") == 0)
                newGameReset(root);
        }

        // se la parola è proprio r e può leggere parole: stampa ok e termina la partita
        else if (n > 0 && strcmp(buffer, riferimento) == 0){
            printf("ok\n");
            n = 0;
        }

        // se può ancora leggere parole (e la parola non era r)
        else if (n > 0){
            // se la parola è ammissibile gioca un round
            if (is_in_trie(buffer, root)){
                strcpy(temp, riferimento); // mette la parola di riferimento in temp
                play_round(temp, buffer);
                memset(counts, 0, 64);
                x = 0;
                validate_trie(root, 0, counts);
                printf("%d\n", x);
                n--; // ho giocato un turno
                if (n == 0) printf("ko\n"); // se non ho più turni a disposizione stampo ko
            }

            // se la parola non è ammissibile
            else printf("not_exists\n");
        }
    }

    return 0;
}
