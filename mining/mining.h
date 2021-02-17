#ifndef _MINING_
#define _MINING_

#include <stdint.h>
#include "../block/block.h"

void mining(Block* pBlock);
void difficulty(uint32_t bits, char* current_target, int n);

#endif