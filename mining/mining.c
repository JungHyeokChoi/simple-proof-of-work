#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../block/block.h"
#include "../sha256/sha256.h"
#include "../utils/utils.h"

void difficulty(uint32_t bits, char* current_target, int n) {
    uint32_t exp = bits >> 24;
    uint32_t mant = bits & 0xffffff;

    int i, j = 5;

    char mant_str[7];

    for(i = 0; i < n - 1; i++)
            current_target[i] = '0';
    current_target[n -1] = '\0';

    sprintf(mant_str, "%06x", mant);

    for(i = (strlen(current_target) - ((8*(exp-3))/4)) - 1; j >= 0; i--) {
            current_target[i] = mant_str[j];
            j--;
    }
}

void mining(Block* pBlock) {
    uint8_t header_str[sizeof(pBlock->header) * 2 + 1];
	uint8_t nonce_str[9];
	uint8_t buf[SHA256_BLOCK_SIZE];
    uint8_t temp[9];

    char difficulty_str[65];

	SHA256_CTX ctx;

	uint32_t nonce = 0;

	int idx;
	int pass = 1;
    int compare_zero_digit;
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

    difficulty(pBlock->header.bits, difficulty_str, sizeof(difficulty_str) / sizeof(char));
    compare_zero_digit = (strlen(difficulty_str) - ((8 * ((pBlock->header.bits >> 24) - 3)) / 4)) - 6;

	while(1) {
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

    pBlock->header.nonce = ConvertHexStrToUint8(nonce_str);
    memmove(pBlock->block_hash, buf, sizeof(buf) / sizeof(uint8_t));

	printf("Result\n");
	printf("Nonce : %d, 0x%x\n", nonce, nonce);
	printf("sha256 : 0x%s\n", ConvertUint8ToHexStr(buf, sizeof(buf)/sizeof(uint8_t)));
}