/*********************************Bismallah******************************************
 * Project by br alwalain: Abdullah Alosaimi, Omar Alsulaiteen, Abdulrahman Alsherefi.
 *
 */

/**************************Header Gaurd**********************************/

#ifndef PROJECT_H
#define PROJECT_H

/*****************************Include************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "server_functions.h"
#include "flags.h"
#include "read_and_write.h"
#include <pthread.h>
#include <semaphore.h>

/*****************************Constant**********************************/


#define LS_BUFFER 16384
#define LIST_SIZE 8
#define MAX_CLIENTS 1024
#define CAT_BUF 2097152
#define MAX 10000


/***********************************************************************/


/****************************Prototype**********************************/

void ls(int connfd, char *line);
void cat(int ,char *l, int32_t flag);
void help(int connfd);
void __exit(void);
void parse_cmd(int connfd, char* mycmd, char* myln);
int strcmp_reverse(char *p1, char *p2);
void err(char *i, const char *fname);
void handler_SIGINT(int i);
void check_malloc(void *p);
void handler_SIGSEGV(int i);
int check_star(char *c);
void skip_leading_spaces(char **strp);
void file_to_buffer(int connfd, char *fname, char *buffer, int retreive_checker, int wild_card_checker);
int client_fd_add(int myfd);
int client_fd_rm(int myfd);
void client_init(void);

/****************************Global Vars*******************************/

//char* cmd;	//command
//char* ln;	//line


//global that contain all clients fds, to allow handlers to handel them.
int clients_fds[MAX_CLIENTS];

int LISTENFD;

#endif
