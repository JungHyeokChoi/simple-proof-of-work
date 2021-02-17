#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "utils.h"

char* ConvertUint8ToHexStr(uint8_t* array, int n) {
    int i;
    char* string = (char*)malloc(n * 2 + 1);

    for(i = 0; i < n; i++) {
        sprintf(&string[strlen(string)], "%02x", array[i]);
    }

    return string;
}

uint8_t* ConvertHexStrToUint8(char* hex_str) {
    int i;
    char temp[3];

    uint8_t* array = (uint8_t*)malloc(strlen(hex_str) / 2);

    for(i = 0; i < strlen(hex_str); i++) {
        memcpy(temp, (hex_str + i * 2), 2);
        temp[2] = '\0';
    
        array[i] = (uint8_t)strtoul(temp, NULL, 16);
    }

    return array;
}

uint32_t timeToHex(time_t sec) {
    char string[8];
    uint32_t hex;

    sprintf(string, "%lx", sec);
    hex = (uint32_t)strtoul(string, NULL, 16);

    return hex;
}