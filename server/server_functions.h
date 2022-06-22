#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "read_and_write.h"
#include "flags.h"
#include <pthread.h>
#include <semaphore.h>

#define BYTES_BUFFER 1024 

void err(char *msg, const char *fname);
char *receive_cmd(int connfd);
void send_reply(int connfd, char *str, int32_t len, int32_t flags);
void cat(int, char *, int32_t flags);
void retrieve(int,  char*);

extern int bytes_sent;
extern int bytes_received;
extern int clients_counter;
extern int retrieved_files;
extern struct timeval TIME;

extern pthread_mutex_t bytes_sent_mutex;
extern pthread_mutex_t bytes_received_mutex;
extern pthread_mutex_t clients_counter_mutex;
extern pthread_mutex_t retrieved_files_mutex;
extern pthread_mutex_t time_mutex;

#endif 
