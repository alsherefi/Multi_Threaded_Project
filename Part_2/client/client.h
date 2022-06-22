/*********************************Bismallah******************************************
 * Project by br alwalain: Abdullah Alosaimi, Omar Alsulaiteen, Abdulrahman Alsherefi.
 *
 */

/**************************Header Gaurd**********************************/

#ifndef CLIENT_H
#define CLIENT_H

/*****************************Include************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "flags.h"
#include "read_and_write.h"

/***********************************************************************/

#define MAX 1024
#define SIZE 4096

/****************************Prototype**********************************/

void __exit(void);
void err(char *);
void handler_SIGINT(int i);
void check_malloc(void *p);
void handler_SIGSEGV(int i);
void send_cmd(int sockfd, char* cmd);
void receive_reply(int sockfd);
char* skip_leading_spaces(char* strp);

/****************************Global Vars*******************************/

extern char *buff;
extern int g_sockfd;

#endif
