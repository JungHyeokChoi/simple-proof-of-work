#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blocklist.h"
#include "common.h"

BlockList* createBlockList(){
	BlockList* pReturn = NULL;
	int i = 0;

	pReturn = (BlockList*)malloc(sizeof(BlockList));

	if (pReturn != NULL) {
		memset(pReturn, 0, sizeof(BlockList));
	}
	else {
		printf("오류, 메모리할당 createBlockList()\n");
		return NULL;
	}

	return pReturn;
}

int addBlock(BlockList* pList, Block block) {
	int ret = false;
	int i = 0;

	Block* pPreBlock = NULL;
	Block* pNewBlock = NULL;

	if (pList != NULL) {
        pNewBlock = (Block*)malloc(sizeof(Block));

        if (pNewBlock != NULL) {
            *pNewBlock = block;
            pNewBlock->pBlock = NULL;

            pPreBlock = &(pList->genesisBlock);
            for (i = 0; i < pList->currentBlockCount; i++) {
                pPreBlock = pNewBlock->pBlock;
            }

            pNewBlock->pBlock = pPreBlock->pBlock;
            pPreBlock->pBlock = pNewBlock;

            pList->currentBlockCount++;

            ret = true;
        }

        else {
            printf("오류, 메모리할당 addBlock()\n");
            return ret;
        }
	}

	return ret;
}

int removeBlock(BlockList* pList, int block_number) {
	int ret = false;
	int i = 0;
	int arrayCount = 0;

	Block* pBlock = NULL;
	Block* pDelBlock = NULL;

	if (pList != NULL) {
		arrayCount = getLinkedListLengh(pList);
		if (block_number >= 0 && block_number < arrayCount) {
			pBlock = &(pList->genesisBlock);
			for (i = 0; i < block_number; i++) {
				pBlock = pBlock->pBlock;
			}

			pDelBlock = pBlock->pBlock;
			pBlock->pBlock = pDelBlock->pBlock;

			free(pDelBlock);

			pList->currentBlockCount--;

			ret = true;

		}
		else {
			printf("오류, 위치 인덱스-[%d] removeBlock()\n", block_number);
		}
	}

	return ret;
}

void deleteBlockList(BlockList* pList) {
	int i = 0;
	if (pList != NULL) {
		clearBlockList(pList);
		free(pList);
	}
}

void clearBlockList(BlockList* pList) {
	if (pList != NULL) {
		if (pList->currentBlockCount > 0) {
			removeBlock(pList, 0);
		}
	}
}

Block* getBlock(BlockList* pList, int block_number) {
    Block* pReturn = NULL;
	int i = 0;

	Block* pBlock = NULL;

	if (pList != NULL) {
		if (block_number >= 0 && block_number < pList->currentBlockCount) {
			pBlock = &(pList->genesisBlock);

			for (i = 0; i <= block_number; i++) {
				pBlock = pBlock->pBlock;
			}
			pReturn = pBlock;
		}
	}

	return pReturn;
}

int getBlockListLengh(BlockList* pList){
	int ret = 0;

	if (pList != NULL) {
		ret = pList->currentBlockCount;
	}
    
	return ret;
}