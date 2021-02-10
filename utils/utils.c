#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* uint8ToString(__uint8_t array[], int n) {
    int i;
    char* result = (char*)malloc(n * 2 + 3);

    strcpy(result, "0x");

    for(i = 0; i != n; ++i) {
        sprintf(&result[strlen(result)], "%x", array[i]);
    }

    return result;
}

__uint8_t* stringToUint8(char array[]) {
    int i;
    __uint8_t result = (__uint8_t)malloc(sizeof(array) / sizeof(char));

    for(i = 0; i < strlen(array); i++) {
        sprintf(result+(i*2), "%02x", array[i]);
    }

    return result;
}