
#include "project.h"


void handler_SIGINT(int sig)
{
	puts("\nExited successfully !!!");
	__exit();
}

void handler_SIGSEGV(int sig)
{
	puts("\nSEGMENTATION FAULT !!! self distructing...\n");
	sleep(1);
	puts("BOOM!\n");
	__exit();
}

void check_malloc(void *p)
{
	if (p != NULL)
		return;

	printf("malloc ERROR: unable to allocate memory\n errno : %s\n", strerror(errno));
	__exit();
}
