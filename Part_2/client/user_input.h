#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void check_malloc(void *p);

typedef struct{
	size_t size;	//array size;
	char *A;	//string (array of char);
} String_a;

String_a cmd;	//command
String_a ln;	//line
