#include <stdio.h>
#include <stdlib.h>

int main(){
    char buffer[8];
    int x;
    while(gets(buffer)){
        x += (int)strtol(buffer, (char **)NULL, 10);
    }
    printf("%d", x);
    return 0;
}