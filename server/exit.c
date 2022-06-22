#include "project.h"

/*
 * this function will be called from the client to exit the program.
 * this function has no return value and takes no arguments.
 */
void __exit(void)
{
	int i, chk;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients_fds[i] > 2)
			close(clients_fds[i]);
	}
	close(LISTENFD);

	exit(0);
}
