#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "utils.h"

char* slice_array(char* array, int start_idx, int end_idx) {
    char* slice_arr = (char*)calloc(end_idx - start_idx + 2, sizeof(char));

    for(int idx = 0; start_idx + idx <= end_idx; idx++)
        slice_arr[idx] = array[start_idx + idx];
    slice_arr[strlen(slice_arr) + 1] = '\0';

    return slice_arr;
}

char* ConvertUint8ToHexStr(uint8_t* array, size_t len) {
    char* string = (char*)calloc(len * 2 + 1, sizeof(char));

    for(int idx = 0; idx < len; idx++) {
        sprintf(&string[strlen(string)], "%02x", array[idx]);
    }
    string[len * 2 + 1] = '\0';

    return string;
}

uint8_t* ConvertHexStrToUint8(char* hex_str) {
    char temp[3];

    uint8_t* array = (uint8_t*)calloc(strlen(hex_str) / 2, sizeof(uint8_t)  );

    for(int idx = 0; idx < strlen(hex_str) / 2; idx++) {
        memcpy(temp, (hex_str + idx * 2), 2);
        temp[2] = '\0';
    
        array[idx] = (uint8_t)strtoul(temp, NULL, 16);
    }

    return array;
}

uint32_t timeToHex(time_t sec) {
    char string[8];

    sprintf(string, "%lx", sec);

    uint32_t hex = (uint32_t)strtoul(string, NULL, 16);

    return hex;
}