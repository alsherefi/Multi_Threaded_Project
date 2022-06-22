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

/****************************Prototype**********************************/

void ls(char *l);
void cat(char *l);
void help(void);
void __exit(void);
void parse_cmd(void);
int strcmp_reverse(char *p1, char *p2);
void err(char *i);
void handler_SIGINT(int i);
void check_malloc(void *p);
void handler_SIGSEGV(int i);
int check_star(char *c);

/*****************************Structures*******************************/

typedef struct{
	size_t size;	//array size;
	char *A;	//string (array of char);
} String_a;

/****************************Global Vars*******************************/

String_a cmd;	//command
String_a ln;	//line

#endif
