#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../block/block.h"
#include "../sha256/sha256.h"
#include "../utils/utils.h"
#include "../common/common.h"

BlockHeader* stringToHeader(char* header_str) {
    BlockHeader* pHeader = malloc(sizeof(struct BlockHeaderType));
    
    char* temp = NULL;

    temp = slice_array(header_str, 0, 7);
    pHeader->version = (uint32_t)strtoul(temp, NULL, 16);

    temp = slice_array(header_str, 8, 71);
    memmove(pHeader->prev_block, ConvertHexStrToUint8(temp), 32);

    temp = slice_array(header_str, 72, 136);
    memmove(pHeader->merkle_root, ConvertHexStrToUint8(temp), 32);

    temp = slice_array(header_str, 137, 144);
    pHeader->timestamp = (uint32_t)strtoul(temp, NULL, 16);

    temp = slice_array(header_str, 145, 152);
    pHeader->bits = (uint32_t)strtoul(temp, NULL, 16);

    temp = slice_array(header_str, 153, 160);
    pHeader->nonce = (uint32_t)strtoul(temp, NULL, 16);

    free(temp);

    return pHeader;
}

uint8_t* headerToString(Block* pBlock, size_t len) {
    uint8_t* header_str = (uint8_t*)calloc(len, sizeof(uint8_t));
    uint8_t temp[9];

    sprintf(temp, "%08x", pBlock->header.version);
    strcat(header_str, temp);
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.prev_block, sizeof(pBlock->header.prev_block) / sizeof(uint8_t)));
    strcat(header_str, ConvertUint8ToHexStr(pBlock->header.merkle_root, sizeof(pBlock->header.merkle_root) / sizeof(uint8_t)));
    sprintf(temp, "%08x", pBlock->header.timestamp);
    strcat(header_str, temp);
    sprintf(temp, "%08x", pBlock->header.bits);
    strcat(header_str, temp);
    sprintf(temp, "%08x", pBlock->header.nonce);
    strcat(header_str, temp);
    
    return header_str;
}

char* difficulty(uint32_t bits, size_t len) {
    uint32_t exp = bits >> 24;
    uint32_t mant = bits & 0xffffff;

    int idx, mant_len;

    char* target_hexstr = (char*)malloc(len * sizeof(char));
    char mant_str[7];

    for(idx = 0; idx < len - 1; idx++)
        target_hexstr[idx] = '0';
    target_hexstr[len - 1] = '\0';

    mant_len = sprintf(mant_str, "%06x", mant);
    mant_len--;

    for(idx = (strlen(target_hexstr) - ((8*(exp-3))/4)) - 1; mant_len >= 0; idx--){
        target_hexstr[idx] = mant_str[mant_len--];
    }

    return target_hexstr;
}

int verify(Block* pBlock) {
    uint8_t* header_str = headerToString(pBlock, BLOCK_HEADER_STRING_SIZE);
	uint8_t sha256[SHA256_BLOCK_SIZE];

	SHA256_CTX ctx;
    
    char* sha256_str;
    char* difficulty_str = difficulty(pBlock->header.bits, SHA256_STRING_BLOCK_SIZE);
    
    int result, pass = 1;
    int compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

    sha256_init(&ctx);
    sha256_update(&ctx, header_str, strlen(header_str));
    sha256_final(&ctx, sha256);

    sha256_str = ConvertUint8ToHexStr(sha256, sizeof(sha256)/sizeof(uint8_t));

    if (strncmp(difficulty_str, sha256_str, compare_zero_digit) == 0) 
        result = true;
    else 
        result = false;

    free(sha256_str);
    free(difficulty_str);

    return result;
}

uint32_t upper_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end) {
    pBlock->header.nonce = nonce_start;

    uint8_t* header_str = headerToString(pBlock, BLOCK_HEADER_STRING_SIZE);
    uint8_t sha256[SHA256_BLOCK_SIZE];

	uint32_t nonce = 0;

	SHA256_CTX ctx;

    char* difficulty_str = difficulty(pBlock->header.bits, SHA256_STRING_BLOCK_SIZE);
	char* sha256_str;
	char nonce_str[9];

	int idx, compare_result, pass = 1;
    int compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

	while(nonce >= nonce_end) {
		sha256_init(&ctx);
		sha256_update(&ctx, header_str, strlen(header_str));
		sha256_final(&ctx, sha256);

		sha256_str = ConvertUint8ToHexStr(sha256, sizeof(sha256)/sizeof(uint8_t));

        compare_result = strncmp(difficulty_str, sha256_str, compare_zero_digit);

        /*
		printf("nonce : %s\n", nonce_str);
		printf("header_str : %s\n", header_str);
		printf("sha256 : 0x%s\n", sha256_str);
        printf("difficulty : 0x%s\n", difficulty_str);
        printf("Compare digit : %d\n", compare_zero_digit);
        printf("Compare Result : %d\n", compare_result);
		printf("\n\n");
        */

		if (compare_result == 0) break;

		nonce--;
		sprintf(nonce_str, "%08x", nonce);

        for(idx = 0; idx < strlen(nonce_str); idx++) 
		    header_str[BLOCK_HEADER_STRING_SIZE - strlen(nonce_str) + idx - 1] = nonce_str[idx];
	}

    memmove(pBlock->block_hash, sha256, sizeof(sha256) / sizeof(uint8_t));

    free(difficulty_str);
    free(sha256_str);

    return nonce;
}

uint32_t lower_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end) {
    pBlock->header.nonce = nonce_start;

    uint8_t* header_str = headerToString(pBlock, BLOCK_HEADER_STRING_SIZE);
    uint8_t sha256[SHA256_BLOCK_SIZE];

	uint32_t nonce = 0;

	SHA256_CTX ctx;

    char* difficulty_str = difficulty(pBlock->header.bits, SHA256_STRING_BLOCK_SIZE);
	char* sha256_str;
	char nonce_str[9];

	int idx, compare_result, pass = 1;
    int compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

	while(nonce <= nonce_end) {
		sha256_init(&ctx);
		sha256_update(&ctx, header_str, strlen(header_str));
		sha256_final(&ctx, sha256);

		sha256_str = ConvertUint8ToHexStr(sha256, sizeof(sha256)/sizeof(uint8_t));

        compare_result = strncmp(difficulty_str, sha256_str, compare_zero_digit);

        /*
		printf("nonce : %s\n", nonce_str);
		printf("header_str : %s\n", header_str);
		printf("sha256 : 0x%s\n", sha256_str);
        printf("difficulty : 0x%s\n", difficulty_str);
        printf("Compare digit : %d\n", compare_zero_digit);
        printf("Compare Result : %d\n", compare_result);
		printf("\n\n");
        */

		if (compare_result == 0) break;

		nonce++;
		sprintf(nonce_str, "%08x", nonce);

        for(idx = 0; idx < strlen(nonce_str); idx++) 
		    header_str[BLOCK_HEADER_STRING_SIZE - strlen(nonce_str) + idx - 1] = nonce_str[idx];
	}

    memmove(pBlock->block_hash, sha256, sizeof(sha256) / sizeof(uint8_t));

    free(difficulty_str);
    free(sha256_str);

    return nonce;
}