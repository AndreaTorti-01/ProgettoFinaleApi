#include <stdio.h>
#include <time.h>
#include <unistd.h>
#define SIZE 6

int main(){
    time_t ts, te;
    char string[SIZE] = "hello";
    int i, r;
    double time_taken[3];

    ts = clock();
    for (i=0; i<32768; i++){
        printf("%s\n", string);
    }
    te = clock();
    time_taken[0] = ((double)(te-ts))/CLOCKS_PER_SEC; // in seconds

    ts = clock();
    for (i=0; i<32768; i++){
        r = write(1, string, sizeof(char) * SIZE);
    }
    te = clock();
    time_taken[1] = ((double)(te-ts))/CLOCKS_PER_SEC; // in seconds

    ts = clock();
    for (i=0; i<32768; i++){
        puts(string);
    }
    te = clock();
    time_taken[2] = ((double)(te-ts))/CLOCKS_PER_SEC; // in seconds

    printf("printf took %f seconds to execute \n", time_taken[0]);
    printf("write took %f %d seconds to execute \n", time_taken[1], r);
    printf("puts took %f seconds to execute \n", time_taken[2]);

    return 0;
}