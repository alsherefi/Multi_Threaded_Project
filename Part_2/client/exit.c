#include "client.h"

void __exit(void)
{	
	if (g_sockfd < 0 )//check if we have a connection
		exit(0);

	send_cmd(g_sockfd, "exit\n");
	receive_reply(g_sockfd);

	close(g_sockfd);

	if (buff != NULL)
		free(buff);

	exit(0);
}
