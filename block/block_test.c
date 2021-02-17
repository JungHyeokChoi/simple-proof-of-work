#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "block.h"
#include "../utils/utils.h"

int main(int argc, char *argv[]) {
    int i = 0;
    int arrayCount = 0;

    time_t sec;
    uint32_t hex;
    uint8_t hex_array[32] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};

    BlockList* pList = NULL;
    Block* pBlock = NULL;
    Block block;

    pList = createBlockList();

    if(pList != NULL) {
        hex = timeToHex(time(&sec));

        memmove(block.block_hash, hex_array, sizeof(hex_array) / sizeof(uint8_t));
        block.header.version = 2;
        memmove(block.header.prev_block, hex_array, sizeof(hex_array) / sizeof(uint8_t));
        memmove(block.header.merkle_root, hex_array, sizeof(hex_array) / sizeof(uint8_t));
        block.header.timestamp = hex;
        block.header.bits = 0x19015f53;
        block.header.nonce = 0;

        addBlock(pList, block);
        printBlock(pList, 0);
        printf("\n");

        hex = timeToHex(time(&sec));

        memmove(block.block_hash, hex_array, sizeof(hex_array) / sizeof(uint8_t));
        block.header.version = 2;
        memmove(block.header.prev_block, hex_array, sizeof(hex_array) / sizeof(uint8_t));
        memmove(block.header.merkle_root, hex_array, sizeof(hex_array) / sizeof(uint8_t));
        block.header.timestamp = hex;
        block.header.bits = 0x19015f53;
        block.header.nonce = 0;

        addBlock(pList, block);
        printBlock(pList, 1);
        printf("\n");

        clearBlockList(pList);
    }
}