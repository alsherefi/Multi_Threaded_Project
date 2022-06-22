#include "project.h"

void help(int connfd)
{
	int bytes = 244;
	char buffer[bytes];

	if (buffer == NULL)
		err("Calloc Error in help function");

	sprintf(buffer, "ls: list files in current remote directory\n");
	sprintf(buffer, "%sls *.txt: list text files\n", buffer);
	sprintf(buffer, "%scat: output file contents\n", buffer);
	sprintf(buffer, "%scat *.c: output C file contents\n", buffer);
	sprintf(buffer, "%sret filename: retrieve filename\n", buffer);
	sprintf(buffer, "%sret *.txt: retrieve all text files\n", buffer);
	sprintf(buffer, "%sstat: print statistics\n", buffer);
	sprintf(buffer, "%sexit: exit the file server\n", buffer);

	send_reply(connfd, buffer, 0);

}
