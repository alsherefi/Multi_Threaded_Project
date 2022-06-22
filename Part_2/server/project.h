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

/***********************************************************************/

#define LS_BUFFER 16384

/****************************Prototype**********************************/

void ls(int connfd, char *line);
void cat(int ,char *l, int32_t flag);
void help(int connfd);
void __exit(void);
void parse_cmd(int);
int strcmp_reverse(char *p1, char *p2);
void err(char *i);
void handler_SIGINT(int i);
void check_malloc(void *p);
void handler_SIGSEGV(int i);
int check_star(char *c);
void skip_leading_spaces(char **strp);

/****************************Global Vars*******************************/

char* cmd;	//command
char* ln;	//line

int LISTENFD;

#endif
