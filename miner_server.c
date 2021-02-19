#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>
#include<pthread.h>

#include "../common/common.h"
#include "../utils/utils.h"
#include "mining.h"

pthread_mutex_t mutex_lock;

BlockList* pList;

typedef struct MiningMultipleArgType {
    int client_sock;
    BlockList* pList;
    Block block
}MiningMultipleArg;

void* read_thread(void *pClient_sock) {
    int size;
    char hex_str[BLOCK_HEADER_STRING_SIZE]; 

    BlockHeader* pHeader = (BlockHeader*)malloc(sizeof(BlockHeader));

    while(1) {
        if ((size = read(*(int*)pClient_sock, hex_str, BLOCK_HEADER_STRING_SIZE)) < 0) {
            printf("Error if read. \n");
        } else {
            hex_str[BLOCK_HEADER_STRING_SIZE] = '\0';

            pHeader = stringToHeader(hex_str);

            if(verify(pHeader) == true) {
                return pHeader;
            };
        }
    }
}

void* mining_thread(void *mining_multiple_arg) {
    int size;
    char hex_str[BLOCK_HEADER_STRING_SIZE];

    MiningMultipleArg *multiple_arg = (MiningMultipleArg*)mining_multiple_arg;

    Block* pBlock = &multiple_arg->block;

    while(1) {
        multiple_arg->block.header.nonce = lower_mining(pBlock, 0x00000000, 0xffffffff);
        pBlock = &multiple_arg->block;
        
        if(verify(pBlock) == true) {
            while(fgets(hex_str, BLOCK_HEADER_STRING_SIZE, stdin) != NULL) {
                size = strlen(hex_str);

                if(write(multiple_arg->client_sock, hex_str, strlen(hex_str)) != size) {
                    printf("Error in write. \n");        
                } else {
                    addBlock(multiple_arg->pList, multiple_arg->block);
                    pthread_exit(NULL);
                }
            }
        }
    }
}

void handler() {
  int state;

  waitpid(-1, &state, WNOHANG);
  exit(0);

  return ;
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    int clntlen, block_number = 0;
    struct sockaddr_in client_addr, server_addr;
    struct sigaction act;
    act.sa_handler = handler;

    void* pHeader;

    pthread_t read_thread, write_thread;

    uint8_t prev_block[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x17,0xc8,0x03,0x78,0xb8,0xda,0x0e,0x33,0x55,0x9b,0x59,0x97,0xf2,0xad,0x55,0xe2,0xf7,0xd1,0x8e,0xc1,0x97,0x5b,0x97,0x17};
    uint8_t merkle_root[32] = {0x87,0x17,0x14,0xdc,0xba,0xe6,0xc8,0x19,0x3a,0x2b,0xb9,0xb2,0xa6,0x9f,0xe1,0xc0,0x44,0x03,0x99,0xf3,0x8d,0x94,0xb3,0xa0,0xf1,0xb4,0x47,0x27,0x5a,0x29,0x97,0x8a};

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

    sigaction(SIGCHLD, &act, 0);

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Server : can't bind local address.\n");
        exit(0);
    }

    printf("Server started. \nWaiting for client.. \n");
    listen(server_sock, 1);

    clntlen = sizeof(client_addr);
    if((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &clntlen)) < 0) {
        printf("Server : failed in accepting. \n");
        exit(0);
    }

    MiningMultipleArg *mining_multiple_arg = (MiningMultipleArg *)malloc(sizeof(MiningMultipleArg));
    mining_multiple_arg->pList = createBlockList();
  
    while(1) {
        printf("Create Block #%d...\n\n", block_number);

        mining_multiple_arg->block.header.version = 0x20800000;
        memmove(mining_multiple_arg->block.header.prev_block, prev_block, sizeof(prev_block) / sizeof(uint8_t));
        memmove(mining_multiple_arg->block.header.merkle_root, merkle_root, sizeof(merkle_root) / sizeof(uint8_t));
        mining_multiple_arg->block.header.timestamp = timeToHex(time(&sec));
        mining_multiple_arg->block.header.bits = 0x1d00ffff;

        pthread_create(&read_thread, NULL, read_thread, &mining_multiple_arg->client_sock);
        pthread_create(&write_thread, NULL, mining_thread, mining_multiple_arg);

        pthread_join(read_thread, &pHeader);
        pthread_join(write_thread, NULL);

        block_number++;
    }

    close(server_sock);
    close(client_sock);

    return 0;
}