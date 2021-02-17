#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "sha256.h"
#include "../utils/utils.h"

int sha256_test()
{
	uint8_t text1[] = {"abc"};
	uint8_t text2[SHA256_BLOCK_SIZE];
	uint8_t buf[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	int idx;
	int pass = 1;

	/*
	uint8_t hash1[SHA256_BLOCK_SIZE] = {0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
	                                 0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};

	sha256_init(&ctx);
	sha256_update(&ctx, text1, strlen(text1));
	sha256_final(&ctx, buf);
	for (int i = 0; i < SHA256_BLOCK_SIZE;) {
		printf ("%02x", buf[i++]);
		if (!(i % 4))
		printf (" ");
  	}
  	printf ("\n");
	pass = pass && !memcmp(hash1, buf, SHA256_BLOCK_SIZE);
	*/

	int nonce = 0;
	char *sha256;
	clock_t start, end;

	start = clock();

	while(1) {
		sprintf(text2, "%d", nonce);

		uint8_t *pText = (uint8_t*)malloc(sizeof(uint8_t)*(strlen(text1) + strlen(text2)));

		strcat(pText, text1);
		strcat(pText, text2);

		sha256_init(&ctx);
		sha256_update(&ctx, pText, strlen(pText));
		sha256_final(&ctx, buf);

		sha256 = ConvertUint8ToHexStr(buf, sizeof(buf)/sizeof(uint8_t));

		printf("nonce : %d\n", nonce);
		printf("pText : %s\n", pText);
		printf("sha256 : 0x%s\n",sha256);
		printf ("\n\n");

		if (strncmp("0000000000", sha256, 4) == 0) break;
		
		free(pText);
		nonce++;
	}
	end = clock();


	printf("Result\n");
	printf("Nonce : %d, 0x%x\n", nonce, nonce);
	printf("shar256 : 0x%s\n", sha256);
	printf("Time: %lfs\n\n",(double)(end - start)/CLOCKS_PER_SEC);

	return(pass);
}

int main()
{
	printf("SHA-256 tests: %s\n", sha256_test() ? "SUCCEEDED" : "FAILED");

	return(0);
}