#ifndef _UTILS_
#define _UTILS_

#include <time.h>
#include <stdint.h>

char* slice_array(char* array, int start_idx, int end_idx);
char* ConvertUint8ToHexStr(uint8_t* array,  size_t len);
uint8_t* ConvertHexStrToUint8(char* hex_str);
uint32_t timeToHex(time_t sec);

#endif