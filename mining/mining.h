#ifndef _MINING_
#define _MINING_

#include <stdint.h>
#include "../block/block.h"

int verify(Block* pBlock);
char* difficulty(uint32_t bits, int n);
uint32_t upper_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end);
uint32_t lower_mining(Block* pBlock, uint32_t nonce_start, uint32_t nonce_end);

#endif