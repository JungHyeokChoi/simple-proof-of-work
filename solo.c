 
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "./mining/mining.h"
#include "./utils/utils.h"
#include "./block/block.h"
#include "./common/common.h"

int main(int argc, char *argv[]) {
    BlockList* pList = NULL;
    Block block;
    Block* pBlock = &block;

    uint8_t prev_block[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x17,0xc8,0x03,0x78,0xb8,0xda,0x0e,0x33,0x55,0x9b,0x59,0x97,0xf2,0xad,0x55,0xe2,0xf7,0xd1,0x8e,0xc1,0x97,0x5b,0x97,0x17};
    uint8_t merkle_root[32] = {0x87,0x17,0x14,0xdc,0xba,0xe6,0xc8,0x19,0x3a,0x2b,0xb9,0xb2,0xa6,0x9f,0xe1,0xc0,0x44,0x03,0x99,0xf3,0x8d,0x94,0xb3,0xa0,0xf1,0xb4,0x47,0x27,0x5a,0x29,0x97,0x8a};

    time_t sec;
    clock_t start, end;

    start = clock();

    pList = createBlockList();

    for(int idx = 0; idx < 20; idx++) {
        printf("Create Block #%d...\n\n", idx);

        block.header.version = 0x20800000;
        memmove(block.header.prev_block, prev_block, sizeof(prev_block) / sizeof(uint8_t));
        memmove(block.header.merkle_root, merkle_root, sizeof(merkle_root) / sizeof(uint8_t));
        block.header.timestamp = timeToHex(time(&sec));
        block.header.bits = 0x1d00ffff;
        block.header.nonce = lower_mining(pBlock, 0x00000000, 0xffffffff);

        pBlock = &block;

        if(verify(pBlock) == true) {
            addBlock(pList, block);
            printBlock(pList, idx);
            printf("\n");

            pBlock = getBlock(pList, idx);

            memmove(prev_block, pBlock->block_hash, sizeof(pBlock->block_hash) / sizeof(uint8_t));
            pBlock = &block;
        } else {
            printf("Fail!! Create Block\n");
            idx--;
        }
    }
    end = clock();

    printf("Time: %lfs\n", (double)(end - start)/CLOCKS_PER_SEC);

    return 0;
}
