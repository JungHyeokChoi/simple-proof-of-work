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

BlockList* pList;
int nonce;
char hex_str[BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2)];

typedef struct MiningMultipleArgType {
    int client_sock;
    Block block;
} MiningMultipleArg;

void* read_block(void *pClient_sock) {
    int size;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    while(1) {

        if ((size = read(*(int*)pClient_sock, hex_str, BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2) - 1)) < 0) {
            printf("Error if read. \n");
        }
        hex_str[BLOCK_HEADER_STRING_SIZE + (SHA256_BLOCK_SIZE * 2) - 1] = '\0';

        if (size > 0) {
            return NULL;
        }
    }
}

void* mining(void *pBlock) {
    uint32_t nonce_start = 0x00000000, nonce_end = 0x00010000;
    Block *pBlock_inner = (Block*)pBlock;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    while(nonce_start <= nonce_end) {
        nonce = lower_mining(pBlock_inner, nonce_start, nonce_end);

        pBlock_inner->header.nonce = nonce;

        // printf("version : %08x\n",pBlock_inner->header.version);
        // printf("prev_block : 0x");
        // for(int i = 0; i< 31; i++)
        //     printf("%02x",pBlock_inner->header.prev_block[i]);
        // printf("\nmerkle_root : 0x");
        // for(int i = 0; i< 31; i++)
        //     printf("%02x",pBlock_inner->header.merkle_root[i]);
        // printf("\ntimestamp : %08x\n",pBlock_inner->header.timestamp);
        // printf("bits : %08x\n",pBlock_inner->header.bits);
        // printf("nonce : %08x\n\n",pBlock_inner->header.nonce);

        if(verify(pBlock_inner) == true) {
            printf("Mining Exit\n");
            break;
        }

        usleep(500);
        
        nonce_start += 0x00010000 + 1;
        nonce_end += 0x00010000;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    int* pClient_sock;
    int clntlen, status, block_number = 0;
    int read_flag, mining_flag;
    struct sockaddr_in client_addr, server_addr;

    pthread_t read_thread, mining_thread;
    pthread_attr_t attr;
 
    uint8_t prev_block[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x17,0xc8,0x03,0x78,0xb8,0xda,0x0e,0x33,0x55,0x9b,0x59,0x97,0xf2,0xad,0x55,0xe2,0xf7,0xd1,0x8e,0xc1,0x97,0x5b,0x97,0x17};
    uint8_t merkle_root[32] = {0x87,0x17,0x14,0xdc,0xba,0xe6,0xc8,0x19,0x3a,0x2b,0xb9,0xb2,0xa6,0x9f,0xe1,0xc0,0x44,0x03,0x99,0xf3,0x8d,0x94,0xb3,0xa0,0xf1,0xb4,0x47,0x27,0x5a,0x29,0x97,0x8a};

    Block* pBlock;
    Block block;

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

    pClient_sock = &client_sock;

    pList = createBlockList();
      
    while(1) {
        printf("Create Block #%d...\n\n", block_number);

        memset(&hex_str, 0, sizeof(hex_str) / sizeof(char));
        nonce = 0x00000000;

        block.header.version = 0x20800000;
        memmove(block.header.prev_block, prev_block, sizeof(prev_block) / sizeof(uint8_t));
        memmove(block.header.merkle_root, merkle_root, sizeof(merkle_root) / sizeof(uint8_t));
        block.header.timestamp = timeToHex(time(&sec));
        block.header.bits = 0x1d00ffff;

        pBlock = &block;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&read_thread, &attr, read_block, pClient_sock);
        pthread_attr_destroy(&attr);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&mining_thread, NULL, mining, pBlock);
        pthread_attr_destroy(&attr);

        read_flag = 0, mining_flag = 0;

        while(1) {
            status = pthread_kill(read_thread, 0);
            if(status == ESRCH) {
                read_flag = 1;

                status = pthread_kill(mining_thread, 0);
                if(status != ESRCH && status != EINVAL) {
                    pthread_cancel(mining_thread);
                }

                usleep(100);
                
                printf("mining_thread exit\n");
                break;
            }

            status = pthread_kill(mining_thread, 0);
            if(status == ESRCH) {
                mining_flag = 1;

                status = pthread_kill(read_thread, 0);
                if(status != ESRCH && status != EINVAL) {
                    pthread_cancel(read_thread);
                }
                usleep(100);

                printf("read_thread exit\n"); 
                break;
            }
        }

        if(read_flag == 1 && mining_flag == 0) {
            BlockHeader* pHeader = (BlockHeader*)malloc(sizeof(BlockHeader));
            Block* pBlock_inner;
            Block block;
            
            uint8_t* sha256;

            char* string;

            string = slice_array(hex_str, 64, 223);
            pHeader = stringToHeader(string);

            free(string);
            string = NULL;
            
            string = slice_array(hex_str, 0, 63);
            sha256 = ConvertHexStrToUint8(string);

            memmove(block.block_hash, sha256, SHA256_BLOCK_SIZE / sizeof(uint8_t));
            block.header.version = pHeader->version; 
            memmove(block.header.prev_block, pHeader->prev_block, sizeof(pHeader->prev_block) / sizeof(uint8_t));
            memmove(block.header.merkle_root, pHeader->merkle_root, sizeof(pHeader->merkle_root) / sizeof(uint8_t));
            block.header.timestamp = pHeader->timestamp;
            block.header.bits = pHeader->bits;
            block.header.nonce= pHeader->nonce;

            pBlock_inner = &block;

            if(verify(pBlock_inner) == true) {
                addBlock(pList, block);
                printf("Create Block by miner_client\n\n");
            }

            free(string);
            free(sha256);
            free(pHeader);
        } else {
            Block* pBlock_inner;

            int size;
            char *sha256_str, *header_str;

            block.header.nonce = nonce;

            pBlock_inner = &block;

            if(verify(pBlock) == true) {
                sha256_str = ConvertUint8ToHexStr(pBlock->block_hash, sizeof(pBlock->block_hash) / sizeof(uint8_t));
                strcat(hex_str, sha256_str);

                header_str = headerToString(pBlock, BLOCK_HEADER_STRING_SIZE);
                strcat(hex_str, header_str);

                size = strlen(hex_str);

                if(write(client_sock, hex_str, strlen(hex_str)) != size) {
                    printf("Error in write. \n");
                } else {
                    addBlock(pList, block);
                    printf("Create Block by miner_server\n\n");
                }
            }

            free(sha256_str);
            free(header_str);
        }

        printBlock(pList, block_number);
        printf("\n");

        pBlock = getBlock(pList, block_number);

        memmove(prev_block, pBlock->block_hash, sizeof(pBlock->block_hash) / sizeof(uint8_t));

        if(pBlock != NULL) {
            free(pBlock);
            pBlock = NULL;
        }

        block_number++;
    }

    close(server_sock);
    close(client_sock);

    return 0;
}