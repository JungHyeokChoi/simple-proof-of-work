#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "sha256.h"

int sha256_test() {
	uint8_t text = {"abc"};
	uint8_t buf[SHA256_BLOCK_SIZE];
	uint8_t hash[SHA256_BLOCK_SIZE] = {0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};

	SHA256_CTX ctx;

	int idx;
	int pass = 1;

	sha256_init(&ctx);
	sha256_update(&ctx, text, strlen(text));
	sha256_final(&ctx, buf);
	
	for (int i = 0; i < SHA256_BLOCK_SIZE;) {
		printf ("%02x", buf[i++]);
		if (!(i % 4))
		printf (" ");
  	}
  	printf ("\n");

	pass = pass && !memcmp(hash, buf, SHA256_BLOCK_SIZE);
	
	return(pass);
}

int main(int argc, char *argv[]) {
	printf("SHA-256 tests: %s\n", sha256_test() ? "SUCCEEDED" : "FAILED");

	return(0);
}