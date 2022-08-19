// lunghezza parole max: 8, numero parole e turni max: 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXWORDLEN 256

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
    } else return false;
}

void cut_end(char* word, uint8_t end){
    for (;end < strlen(word); end++) word[end] = '\0';
}

void slice(char *str, char *result, size_t start, size_t end){
    strncpy(result, str + start, end - start);
}

// ritorna la lunghezza del pezzo iniziale che corrisponde, 0 se non corrisponde, -1 se corrisponde fino alla fine di trieChunk
int8_t match_until(char* trieChunk, char* inputWord){
    int8_t num = 0;
    while(trieChunk[num] != '\0'){
        if (inputWord[num] != trieChunk[num]) return num;
        num++;
    } 
    return -1;
}

node_ptr trie_insert(char* word, node_ptr root){
    node_ptr prev = NULL;
    node_ptr temp = root;
    int8_t nMatch;
    uint8_t wordLen = strlen(word);

    while(1){
        if (temp != NULL) nMatch = match_until(temp->chunk + 1, word);
        else nMatch = 0;

        if (nMatch > 0){ // deve inserire in temp->down wordRemainder e trieRemainder, ovvero le parti che non corrispondono
            uint8_t chunkLen = strlen(temp->chunk + 1);
            
            char wordRemainder[wordLen - nMatch + 1];
            char trieRemainder[chunkLen - nMatch + 2];
            slice(word, wordRemainder, nMatch, wordLen + 1); // mette in wordRemainder i caratteri di word che non corrispondevano
            slice(temp->chunk + 1, trieRemainder, nMatch, chunkLen + 2); // mette in trieRemainder i caratteri di chunk che non corrispondevano

            temp->chunk = realloc(temp->chunk, sizeof(char) * (nMatch + 2)); // rimpicciolisce il nodo già presente nel trie
            temp->chunk[nMatch + 1] = '\0';
            prev = temp->down;
            temp->down = malloc(sizeof(struct node));  // inserisce il pezzo rimasto sotto
            temp->down->down = prev;
            temp->down->chunk = malloc(sizeof(char) * (chunkLen - nMatch + 2));
            strcpy(temp->down->chunk + 1, trieRemainder);
            temp->down->chunk[0] = temp->chunk[0]; // eredita la validità dal pezzo sopra
            temp->chunk[0] = true; // imposta il pezzo sopra come valido
            temp->down = trie_insert(wordRemainder, temp->down); // ed ora inserisce il remainder della word nel livello sotto
            return root;
        }
        else if (nMatch == -1){ // se invece corrisponde tutto dobbiamo solo inserire il wordRemainder al livello sotto
            uint8_t chunkLen = strlen(temp->chunk + 1);
            char wordRemainder[wordLen - chunkLen + 1];
            slice(word, wordRemainder, chunkLen, wordLen + 1);
            temp->down = trie_insert(wordRemainder, temp->down);
            return root;
        }
        // se non riesce a inserire
        if (temp == NULL){ // magari è la fine della lista
            temp = malloc(sizeof(struct node));
            temp->right = temp->down = NULL;
            if (prev != NULL) prev->right = temp; // solo se non è il primo inserimento in assoluto
            temp->chunk = malloc(sizeof(char) * (wordLen + 2));
            strcpy(temp->chunk + 1, word);
            temp->chunk[0] = true;
            if (prev == NULL) root = temp; // solo se è il primo inserimento in assoluto
            return root;
        }
        if (temp->chunk[1] > word[0]){ // oppure sto inserendo prima della fine
            if (prev == NULL) { // magari all'inizio
                prev = malloc(sizeof(struct node));
                prev->right = temp;
                prev->down = NULL;
                prev->chunk = malloc(sizeof(char) * (wordLen + 2));
                strcpy(prev->chunk + 1, word);
                prev->chunk[0] = true;
                return prev;
            }
            else{ // oppure in mezzo
                temp = malloc(sizeof(struct node));
                temp->right = prev->right;
                prev->right = temp;
                temp->down = NULL;
                temp->chunk = malloc(sizeof(char) * (wordLen + 2));
                strcpy(temp->chunk + 1, word);
                temp->chunk[0] = true;
                return root;
            }
        }
        prev = temp;
        temp = temp->right; // scorre a destra
        
    }
}

void print_trie(char* passed, uint8_t index, node_ptr root){
    if (root->down == NULL){ // se sono in una foglia
        if (root->chunk[0]) printf("%s%s\n", passed, root->chunk + 1);
        if (root->right != NULL) print_trie(passed, index, root->right);
    }
    else{ // se sono in mezzo
        if (root->chunk[0]){
            char temp[strlen(passed) + 1];
            cut_end(passed, index);
            strcpy(temp, passed);
            strcat(temp, root->chunk + 1);
            index += strlen(root->chunk + 1);
            print_trie(temp, index, root->down);
        }
        if (root->right != NULL) print_trie(passed, index, root->right);
    }
}

bool is_in_trie(char* word, node_ptr root){
    int8_t nMatch;
    int8_t wordLen = strlen(word);
    char tempWord[k + 1];
    while (root != NULL){ // scorre a destra fino alla fine
        nMatch = match_until(root->chunk + 1, word);
        if (nMatch == -1){
            if (strlen(root->chunk + 1) == wordLen && root->down == NULL) return true;
            slice(word, tempWord, strlen(root->chunk + 1), wordLen + 1);
            return is_in_trie(tempWord, root->down);
        }
        else if (nMatch > 0){
            slice(word, tempWord, nMatch, wordLen + 1);
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
        uint8_t tempMap = map(p[i]);
        if (output[i] != '+'){
            constraints[tempMap].bannedInPos[i] = true; // la banno da quella posizione
            found = false;

            // cerco la lettera nella parola di riferimento
            for (j = 0; j < k && found == false; j++){
                if (p[i] == r[j]){ // trovata
                    found = true;
                    output[i] = '|';
                    r[j] = '?'; // la uso
                    occs[tempMap]++; // aumento di uno il numero di occorrenze trovate di tale lettera
                }
            }

            if (found == false){ // e se non l'ho trovata...
                if (occs[tempMap] == 0) // potrei non averla mai trovata
                    for (j = 0; j < k; j++) constraints[tempMap].bannedInPos[j] = true; // la banno ovunque
                else // o magari l'ho trovata in precedenza
                    constraints[tempMap].Occ = occs[tempMap]; // allora adesso conosco il numero di occorrenze esatto!
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
    uint8_t tempMap;
    memcpy(temp, counts, sizeof(uint8_t) * 64);

    if (root->chunk[0]){ // se il nodo è ancora valido...
        uint8_t i, len = strlen(root->chunk + 1) + 1;
        if (root->down == NULL){ // se sono in una foglia
            for (i=1; i<len && root->chunk[0]; i++){ // per ogni lettera
                tempMap = map(root->chunk[i]);
                counts[tempMap]++;
                if ((guessedChars[i+startIndex-1] != '?' && root->chunk[i] != guessedChars[i+startIndex-1]) || // se il char è in una posizione indovinata e non è quello corretto
                    (constraints[tempMap].bannedInPos[i+startIndex-1] == true)){ // se il char è bannato da quella posizione
                    root->chunk[0] = false;
                }
            }
            for (i=0; i<64 && root->chunk[0]; i++){
                if ((constraints[i].Occ != 0 && counts[i] != constraints[i].Occ) || // se non ho il numero esatto di occorrenze
                    (constraints[i].minOcc != 0 && counts[i] < constraints[i].minOcc)){ // oppure il numero minimo
                    root->chunk[0] = false;
                }
            }
            if (root->chunk[0]) x++; // se sono arrivato fin qui ho trovato una parola valida
        }
        else{ // se sono in mezzo
            for (i=1; i<len && root->chunk[0]; i++){ // cerco di invalidare il nodo
                tempMap = map(root->chunk[i]);
                counts[tempMap]++;
                if ((guessedChars[i+startIndex-1] != '?' && root->chunk[i] != guessedChars[i+startIndex-1]) || // se il char è in una posizione indovinata e non è quello corretto
                    (constraints[tempMap].bannedInPos[i+startIndex-1] == true) || // se il char è bannato da quella posizione
                    (constraints[tempMap].Occ != 0 && counts[tempMap] > constraints[tempMap].Occ)){ // se ho superato il numero di occorrenze esatto
                    root->chunk[0] = false;
                }
            }
            if (root->chunk[0]) validate_trie(root->down, startIndex + len - 1, counts); // inizio a validare di sotto
        }
    }

    if (root->right != NULL){ // sicuramente vanno validate le parole a destra
        validate_trie(root->right, tempIndex, temp);
    }
}

void revalidate_trie(node_ptr root){
    root->chunk[0] = true;
    if (root->down != NULL) revalidate_trie(root->down);
    if (root->right != NULL) revalidate_trie(root->right);
}

void newGameReset (node_ptr root){
    uint8_t i;

    // reimposta x al numero di parole
    x = totalWords;

    // rivalida tutto il dizionario
    revalidate_trie(root);

    // azzera guessedChars e i vincoli
    for (i = 0; i < k; i++)
        guessedChars[i] = '?';
    for (i = 0; i < 64; i++){
        memset(constraints[i].bannedInPos, 0, k);
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

    bool exitBool; // variabili di supporto
    uint8_t i; // variabili di supporto

    // imposta k
    readline();
    k = (uint8_t)strtol(buffer, (char **)NULL, 10);

    // crea un array di supporto e alloca gli spazi necessari scoperti
    char temp[k + 1];
    guessedChars = malloc(sizeof(char) * (k + 1));
    riferimento = malloc(sizeof(char) * (k + 1));

    // crea il dizionario e imposta il numero di parole totali
    exitBool = false;
    totalWords = 0;
    while (!exitBool){
        readline();
        if (buffer[0] != '+'){
            root = trie_insert(buffer, root);
            totalWords++;
        }
        else if (strcmp(buffer, "+inserisci_inizio") != 0 && strcmp(buffer, "+inserisci_fine") != 0)
            exitBool = true;
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
                if (exitBool){
                    memset(counts, 0, 64);
                    x = 0;
                    validate_trie(root, 0, counts);
                }

                memset(temp, '\0', k+1);
                print_trie(temp, 0, root);
            }

            // popola ulteriormente la lista di parole ammissibili
            else if (strcmp(buffer, "+inserisci_inizio") == 0){
                exitBool = false;
                while (exitBool == false){
                    readline();
                    if (buffer[0] != '+'){
                        root = trie_insert(buffer, root);
                        totalWords++;
                    }
                    else
                        exitBool = true;
                }
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
                exitBool = false;
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
