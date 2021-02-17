#ifndef _UTILS_
#define _UTILS_

#include <time.h>
#include <stdint.h>

char* ConvertUint8ToHexStr(uint8_t* array, int n);
uint8_t* ConvertHexStrToUint8(char* hex_str);
uint32_t timeToHex(time_t sec);

#endif