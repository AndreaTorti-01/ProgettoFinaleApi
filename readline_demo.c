#include <stdio.h>
#include <string.h>

FILE* fileptr;
char buffer[64];

int readline(){
    if (fgets(buffer, 64, fileptr)){
        buffer[strcspn(buffer, "\r\n")] = '\0'; // pulisce il buffer dal newline
        return 1;
    }
    else return 0;
}

int main(){
    fileptr = fopen("input.txt", "r");
    while (readline()){
        printf("\n%s\n", buffer);
    }
    return 0;
}
