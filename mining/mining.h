#ifndef _MINING_
#define _MINING_

#include <stdint.h>
#include "../block/block.h"

uint8_t* getHeaderString(Block* pBlock, size_t len);
int verify(Block* pBlock);
char* difficulty(uint32_t bits, size_t len);
uint32_t upper_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end);
uint32_t lower_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end);

#endif