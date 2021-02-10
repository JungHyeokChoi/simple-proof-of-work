#ifndef _BLOCKLIST_
#define _BLOCKLIST_

typedef struct BlockHeaderType {
    __uint8_t version[4];
    __uint8_t prev_block[32];
    __uint8_t merkle_root[32];
    __uint8_t timestamp[4];
    __uint8_t bits[4];
    __uint8_t nonce[4]; 
}BlockHeader;

typedef struct BlockBodyType {
    /* empty */
}BlockBody;

typedef struct BlockType {
    BlockHeader header;
    BlockBody body;
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

#endif _BLOCKLIST_