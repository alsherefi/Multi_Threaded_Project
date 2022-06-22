#include "client.h"

/*
 * this function to print the error message and then exit the program.
 * this function has no return value and takes this arguments:
 * char *msg: the message.
 * char *fname: the function caller name
 */
void err(char *msg, const char *fname)
{
	fprintf(stderr, "%s: %s: %s\n", fname, msg, errno == 0 ? "\b\b " : strerror(errno));
	__exit();
}

/*
 * this function for handling the sigINT signal if it received in the program to exit the program successfully without any errors.
 * this function has no return value and takes this argument:
 * int sig: the signal number.
 */
void handler_SIGINT(int sig)
{
	puts("\nExited successfully !!!");
	__exit();
}

/*
 * this function for handling the sigSEGV signal if it received in the program to know there is segmentation fault and then exit the program.
 * this function has no return value and takes this argument:
 * int sig: the signal number.
 */
void handler_SIGSEGV(int sig)
{
	puts("\nSEGMENTATION FAULT !!! self distructing...\n");
	sleep(1);
	puts("BOOM!\n");
	__exit();
}

/*
 * this function is for checking the malloc if it is successfully done or there are some errors then exit the program
 * this function has no return value and takes this argument:
 * void *p: a pointer.
 */
void check_malloc(void *p)
{
	if (p != NULL)
		return;

	printf("malloc ERROR: unable to allocate memory\n errno : %s\n", strerror(errno));
	__exit();
}
