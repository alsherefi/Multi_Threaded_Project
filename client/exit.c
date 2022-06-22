#include "client.h"

/*
 * this function to exit the program and checks if we have a connection or not.
 * this function has no return values and takes no arguments.
 */
void __exit(void)
{
	if (buff != NULL)
		free(buff);
	if (g_sockfd < 0) //check if we have a connection
		exit(0);

	send_cmd(g_sockfd, "exit\n");
	receive_reply(g_sockfd);

	close(g_sockfd);
	exit(0);
}
