#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../block/block.h"
#include "../sha256/sha256.h"
#include "../utils/utils.h"
#include "../common/common.h"

char* difficulty(uint32_t bits, int n) {
    uint32_t exp = bits >> 24;
    uint32_t mant = bits & 0xffffff;

    int i, j = 5;

    char mant_str[7];
    char* current_target = (char*)malloc(n);

    for(i = 0; i < n - 1; i++)
            current_target[i] = '0';
    current_target[n -1] = '\0';

    sprintf(mant_str, "%06x", mant);

    for(i = (strlen(current_target) - ((8*(exp-3))/4)) - 1; j >= 0; i--) {
            current_target[i] = mant_str[j];
            j--;
    }

    return current_target;
}

int verify(Block* pBlock) {
    uint8_t header_str[BLOCK_HEADER_STRING_SIZE];
	uint8_t buf[SHA256_BLOCK_SIZE];
    uint8_t temp[9];

	uint32_t nonce = 0;

	SHA256_CTX ctx;
    	
    int compare_zero_digit, result, pass = 1;
	
    char* sha256;
    char* difficulty_str = difficulty(pBlock->header.bits, SHA256_STRING_BLOCK_SIZE);

    memset(header_str, 0, sizeof(header_str));

    sprintf(temp, "%08x", pBlock->header.version);
    strcat(header_str, temp);
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.prev_block, sizeof(pBlock->header.prev_block) / sizeof(uint8_t)));
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.merkle_root, sizeof(pBlock->header.merkle_root) / sizeof(uint8_t)));
    sprintf(temp, "%08x", pBlock->header.timestamp);
    strcat(header_str, temp);
    sprintf(temp, "%08x", pBlock->header.bits);
    strcat(header_str, temp);
    sprintf(temp, "%08x", nonce);
    strcat(header_str, temp);

    compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

    sha256_init(&ctx);
    sha256_update(&ctx, header_str, strlen(header_str));
    sha256_final(&ctx, buf);

    if (strncmp(difficulty_str, ConvertUint8ToHexStr(buf, sizeof(buf)/sizeof(uint8_t)), compare_zero_digit) == 0) 
        result = true;
    else 
        result = false;

    free(sha256);
    free(difficulty_str);

    return result;
}

uint32_t upper_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end) {
    uint8_t header_str[BLOCK_HEADER_STRING_SIZE];
	uint8_t nonce_str[9];
	uint8_t buf[SHA256_BLOCK_SIZE];
    uint8_t temp[9];

	uint32_t nonce = nonce_start;

	SHA256_CTX ctx;

	int compare_zero_digit, pass = 1;
    char* difficulty_str = difficulty(pBlock->header.bits, SHA256_STRING_BLOCK_SIZE);
	char *sha256;

    memset(header_str, 0, sizeof(header_str));

    sprintf(temp, "%x", pBlock->header.version);
    strcat(header_str, temp);
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.prev_block, sizeof(pBlock->header.prev_block) / sizeof(uint8_t)));
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.merkle_root, sizeof(pBlock->header.merkle_root) / sizeof(uint8_t)));
    sprintf(temp, "%x", pBlock->header.timestamp);
    strcat(header_str, temp);
    sprintf(temp, "%x", pBlock->header.bits);
    strcat(header_str, temp);

    compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

	while(nonce >= nonce_end) {
		sprintf(nonce_str, "%08x", nonce);

		uint8_t *pHeader_str = (uint8_t*)malloc(sizeof(uint8_t)*(strlen(header_str) + strlen(nonce_str) - 1));

		strcat(pHeader_str, header_str);
		strcat(pHeader_str, nonce_str);

		sha256_init(&ctx);
		sha256_update(&ctx, pHeader_str, strlen(pHeader_str));
		sha256_final(&ctx, buf);

		sha256 = ConvertUint8ToHexStr(buf, sizeof(buf)/sizeof(uint8_t));

		printf("nonce : %x\n", nonce);
		printf("pHeader_str : %s\n", pHeader_str);
		printf("sha256 : 0x%s\n", sha256);
        printf("difficulty : 0x%s\n", difficulty_str);
        printf("Compare digit : %d\n", compare_zero_digit);
        printf("Compare Result : %d\n", strncmp(difficulty_str, sha256, compare_zero_digit));
		printf("\n\n");

        free(sha256);
		free(pHeader_str);

		if (strncmp(difficulty_str, sha256, compare_zero_digit) == 0) break;
	
		nonce--;
	}

    nonce = (uint32_t)strtoul(nonce_str, NULL, 16);

    free(difficulty_str);

    return nonce;
}

uint32_t lower_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end) {
    uint8_t header_str[BLOCK_HEADER_STRING_SIZE];
	uint8_t nonce_str[9];
	uint8_t buf[SHA256_BLOCK_SIZE];
    uint8_t temp[9];

	uint32_t nonce = 0;

	SHA256_CTX ctx;

	int compare_zero_digit, pass = 1;
    char* difficulty_str = difficulty(pBlock->header.bits, SHA256_STRING_BLOCK_SIZE);
	char *sha256;

    memset(header_str, 0, sizeof(header_str));

    sprintf(temp, "%x", pBlock->header.version);
    strcat(header_str, temp);
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.prev_block, sizeof(pBlock->header.prev_block) / sizeof(uint8_t)));
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.merkle_root, sizeof(pBlock->header.merkle_root) / sizeof(uint8_t)));
    sprintf(temp, "%x", pBlock->header.timestamp);
    strcat(header_str, temp);
    sprintf(temp, "%x", pBlock->header.bits);
    strcat(header_str, temp);

    compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

	while(nonce <= nonce_end) {
		sprintf(nonce_str, "%08x", nonce);

		uint8_t *pHeader_str = (uint8_t*)malloc(sizeof(uint8_t)*(strlen(header_str) + strlen(nonce_str) - 1));

		strcat(pHeader_str, header_str);
		strcat(pHeader_str, nonce_str);

		sha256_init(&ctx);
		sha256_update(&ctx, pHeader_str, strlen(pHeader_str));
		sha256_final(&ctx, buf);

		sha256 = ConvertUint8ToHexStr(buf, sizeof(buf)/sizeof(uint8_t));

		printf("nonce : %x\n", nonce);
		printf("pHeader_str : %s\n", pHeader_str);
		printf("sha256 : 0x%s\n", sha256);
        printf("difficulty : 0x%s\n", difficulty_str);
        printf("Compare digit : %d\n", compare_zero_digit);
        printf("Compare Result : %d\n", strncmp(difficulty_str, sha256, compare_zero_digit));
		printf("\n\n");

        free(sha256);
		free(pHeader_str);

		if (strncmp(difficulty_str, sha256, compare_zero_digit) == 0) break;
	
		nonce++;
	}

    nonce = (uint32_t)strtoul(nonce_str, NULL, 16);

    free(difficulty_str);

    return nonce;
}