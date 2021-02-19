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

#include "mining_datastream.h"
#include "../common/common.h"
#include "../utils/utils.h"
#include "mining.h"

void* read_str(void *pClient_sock) {
    int size, flag;
    char hex_str[MAXLINE]; 

    while(flag = true) {
        if ((size = read(*(int*)pClient_sock, hex_str, MAXLINE)) < 0) {
            printf("Error if read. \n");
        } else {
            hex_str[size] = '\0';

            stringToHeader()
        }
    }
    
    
}

void* write_str(void *pClient_sock) {
    int size;
    char hex_str[MAXLINE];

    while(1) {
        while(fgets(hex_str, MAXLINE, stdin) != NULL) {
            size = strlen(hex_str);

            if(write(*(int*)pClient_sock, hex_str, strlen(hex_str)) != size) {
                printf("Error in write. \n");        
            }

            if(strstr(hex_str, escapechar) != NULL) {
                printf("Good bye.\n");
                return NULL;
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