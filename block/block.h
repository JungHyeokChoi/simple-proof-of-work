#ifndef _BLOCK_
#define _BLOCK_

#include <stdint.h>

typedef struct BlockHeaderType {
    int32_t version; // Hex
    uint8_t prev_block[32]; // Hex
    uint8_t merkle_root[32]; // Hex
    uint32_t timestamp; // Hex
    uint32_t bits; // Hex
    uint32_t nonce; // Hex
}BlockHeader;

typedef struct BlockTransactionType {
    /* empty */
}BlockTransaction;

typedef struct BlockType {
    uint8_t block_hash[32];
    BlockHeader header;
    BlockTransaction transaction;
    struct BlockType* pBlock;
}Block;

typedef struct BlockListType {
    int currentBlockCount;
    Block genesisBlock;
}BlockList;

BlockList* createBlockList();
int addBlock(BlockList* pList, Block block);
int removeBlock(BlockList* pList, int block_number);
void deleteBlockList(BlockList* pList);
void clearBlockList(BlockList * pList);
Block* getBlock(BlockList* pList, int block_number);
int getBlockListLengh(BlockList* pList);
void printBlock(BlockList* pList, int block_number);

#endif