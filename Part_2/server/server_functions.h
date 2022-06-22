#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "read_and_write.h"
#include "flags.h"

#define BYTES_BUFFER 1024 

void err(char *msg);
char *receive_cmd(int connfd);
void send_reply(int connfd, char *str, int32_t flags);
void cat(int, char *, int32_t flags);
void retrieve(int,  char*);

extern int bytes_sent;
extern int bytes_received;
extern int clients_counter;
extern int retrieved_files;
extern struct timeval TIME;

#endif 
