#include "project.h"

/*
 * this function will have a menu of the commands and then will give the result to the client to be printed there.
 * this function has no return value and the argumnets that it takes is:
 * int connfd: the client file descriptor.
 */
void help(int connfd)
{
	int bytes = 244;
	char buffer[bytes];
	int s;

	bzero(buffer, bytes);

	sprintf(buffer, "ls: list files in current remote directory\n");
	sprintf(buffer, "%sls *.txt: list text files\n", buffer);
	sprintf(buffer, "%scat: output file contents\n", buffer);
	sprintf(buffer, "%scat *.c: output C file contents\n", buffer);
	sprintf(buffer, "%sret filename: retrieve filename\n", buffer);
	sprintf(buffer, "%sret *.txt: retrieve all text files\n", buffer);
	sprintf(buffer, "%sstat: print statistics\n", buffer);
	s = sprintf(buffer, "%sexit: exit the file server\n", buffer);
	send_reply(connfd, buffer, s, 0);

}
