#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE 6

int main(){
    time_t ts, te;
    char string[SIZE] = "hello";
    int i, j;
    char stringcpy[SIZE];
    double time_taken;

    ts = clock();
    for (i=0; i<32768; i++){
        strcpy(stringcpy, string);
    }
    te = clock();
    time_taken = ((double)(te-ts))/CLOCKS_PER_SEC; // in seconds
    printf("strcpy took %f seconds to execute \n", time_taken);

    ts = clock();
    for (i=0; i<32768; i++){
        strncpy(stringcpy, string, SIZE);
    }
    te = clock();
    time_taken = ((double)(te-ts))/CLOCKS_PER_SEC; // in seconds
    printf("strncpy took %f seconds to execute \n", time_taken);

    ts = clock();
    for (i=0; i<32768; i++){
        memcpy(stringcpy, string, sizeof(char)*SIZE);
    }
    te = clock();
    time_taken = ((double)(te-ts))/CLOCKS_PER_SEC; // in seconds
    printf("custom took %f seconds to execute \n", time_taken);

    printf ("%s\n", stringcpy);
    return 0;
}