#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <errno.h> 

#include "./common/common.h"
#include "./sha256/sha256.h"
#include "./utils/utils.h"
#include "./mining/mining.h"

pthread_mutex_t mutex;

BlockList* pList;

typedef struct MiningMultipleArgType {
    int client_sock;
    Block block;
} MiningMultipleArg;

void* read_blockheader(void *pClient_sock) {
    int size;
    char hex_str[BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2)];

    BlockHeader* pHeader = (BlockHeader*)malloc(sizeof(BlockHeader));
    Block* pBlock;
    Block block;

    while(1) {
        if ((size = read(*(int*)pClient_sock, hex_str, BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2))) < 0) {
            printf("Error if read. \n");
        }
        if (mutex.__data.__lock == 0) {
            pthread_mutex_lock(&mutex);

            hex_str[BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2)] = '\0';

            pHeader = stringToHeader(slice_array(hex_str, 64, 224));

            memmove(block.block_hash, ConvertHexStrToUint8(slice_array(hex_str, 0, 63)), SHA256_BLOCK_SIZE / sizeof(uint8_t));
            block.header.version = pHeader->version; 
            memmove(block.header.prev_block, pHeader->prev_block, sizeof(pHeader->prev_block) / sizeof(uint8_t));
            memmove(block.header.merkle_root, pHeader->merkle_root, sizeof(pHeader->merkle_root) / sizeof(uint8_t));
            block.header.timestamp = pHeader->timestamp;
            block.header.bits = pHeader->bits;
            block.header.nonce= pHeader->nonce;

            pBlock = &block;

            if(verify(pBlock) == true) {
                addBlock(pList, block);
                printf("\nCreate Block by miner_client\n\n\n");
            }
            pthread_mutex_unlock(&mutex);

            free(pHeader);

            return NULL;
        }
    }
}

void* mining(void *mining_multiple_arg) {
    int size;
    char hex_str[BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2)];

    MiningMultipleArg *multiple_arg = (MiningMultipleArg*)mining_multiple_arg;
    Block* pBlock = &multiple_arg->block;

    multiple_arg->block.header.nonce = lower_mining(pBlock, 0x00000000, 0xffffffff);
    pBlock = &multiple_arg->block;
    
    if(verify(pBlock) == true && mutex.__data.__lock == 0) {
        pthread_mutex_lock(&mutex);

        strcat(hex_str, ConvertUint8ToHexStr(pBlock->block_hash, sizeof(pBlock->block_hash) / sizeof(uint8_t)));
        strcat(hex_str, headerToString(pBlock, BLOCK_HEADER_STRING_SIZE));

        size = strlen(hex_str);

        if(write(multiple_arg->client_sock, hex_str, strlen(hex_str)) != size) {
            printf("Error in write. \n");
        } else {
            addBlock(pList, multiple_arg->block);
            printf("\nCreate Block by miner_server\n\n\n");
        }
    }

    if(mutex.__data.__lock == 1) pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    int clntlen, status, block_number = 0;
    struct sockaddr_in client_addr, server_addr;

    pthread_t read_thread, mining_thread;
    pthread_attr_t attr;

    uint8_t prev_block[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x17,0xc8,0x03,0x78,0xb8,0xda,0x0e,0x33,0x55,0x9b,0x59,0x97,0xf2,0xad,0x55,0xe2,0xf7,0xd1,0x8e,0xc1,0x97,0x5b,0x97,0x17};
    uint8_t merkle_root[32] = {0x87,0x17,0x14,0xdc,0xba,0xe6,0xc8,0x19,0x3a,0x2b,0xb9,0xb2,0xa6,0x9f,0xe1,0xc0,0x44,0x03,0x99,0xf3,0x8d,0x94,0xb3,0xa0,0xf1,0xb4,0x47,0x27,0x5a,0x29,0x97,0x8a};

    Block* pBlock;

    time_t sec;

    if(argc != 2) {
        printf("Usage : %s PORT \n", argv[0]);
        exit(0);
    }

    if((server_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        printf("Server : can't open stream socket. \n");
        exit(0);
    }

    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Server : can't bind local address.\n");
        exit(0);
    }

    printf("Server started. \nWaiting for client.. \n\n");
    listen(server_sock, 1);

    clntlen = sizeof(client_addr);
    if((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &clntlen)) < 0) {
        printf("Server : failed in accepting. \n");
        exit(0);
    }

    pList = createBlockList();

    MiningMultipleArg *mining_multiple_arg = (MiningMultipleArg *)malloc(sizeof(MiningMultipleArg));
    mining_multiple_arg->client_sock = client_sock;
      
    while(1) {
        printf("Create Block #%d...\n\n", block_number);

        mining_multiple_arg->block.header.version = 0x20800000;
        memmove(mining_multiple_arg->block.header.prev_block, prev_block, sizeof(prev_block) / sizeof(uint8_t));
        memmove(mining_multiple_arg->block.header.merkle_root, merkle_root, sizeof(merkle_root) / sizeof(uint8_t));
        mining_multiple_arg->block.header.timestamp = timeToHex(time(&sec));
        mining_multiple_arg->block.header.bits = 0x1d00ffff;

        pthread_mutex_init(&mutex, NULL);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&read_thread, &attr, read_blockheader, &mining_multiple_arg->client_sock);
        pthread_attr_destroy(&attr);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&mining_thread, NULL, mining, mining_multiple_arg);
        pthread_attr_destroy(&attr);

        while(1) {
            status = pthread_kill(read_thread, 0);
            if(status == ESRCH) {
                pthread_cancel(mining_thread);
                break;
            }

            status = pthread_kill(mining_thread, 0);
            if(status == ESRCH) {
                pthread_cancel(read_thread);
                break;
            }
        }
        printBlock(pList, block_number);
        printf("\n");

        pBlock = getBlock(pList, block_number);

        memmove(prev_block, pBlock->block_hash, sizeof(pBlock->block_hash) / sizeof(uint8_t));

        block_number++;
    }

    pthread_mutex_destroy(&mutex);

    close(server_sock);
    close(client_sock);

    return 0;
}