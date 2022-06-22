#include "server_functions.h"

char *receive_cmd(int connfd)
{

	int bytes;
	char *command;
	int32_t size;

	if (readn(connfd, &size, sizeof(int32_t)) < 0)
		err("Error in reading the file size in recive cmd");
	
	command = calloc(size, 1);

	if (command == NULL)
		err("Caloc error in command (receive_cmd)");

	if(readn(connfd, command, size) < 0)
		err("Error reading command in receive_cmd");

	bytes_received += size + sizeof(int32_t);
	return command;

}


void send_reply(int connfd, char *str, int32_t flags)
{
	int32_t len;
	int bytes;

	// Sending flag
	if (writen(connfd, &flags, sizeof(int32_t)) < 0)
		err("send_reply: Error writing flags");

	if (flags == IS_FINISHED)
		return;

	len = strlen(str);
	bytes = len;

	// Sending the file size to the client.
	if (writen(connfd, &len, sizeof(int32_t)) < 0)
		err("Error sending the file size in send_reply");

	//Sending every 1024 bytes(for now) to the client.
	while (len > BYTES_BUFFER) {
		if (writen(connfd, str, BYTES_BUFFER) < 0)
			err("Error sending string in sendreply");
		str += BYTES_BUFFER;
		len -= BYTES_BUFFER;
	}

	// Sending the last bytes.
	if (writen(connfd, str, len) < 0)
		err("Error sending number of the last bytes in send_reply");

	bytes_sent += bytes + sizeof(int);
	
}

void retrieve(int connfd, char* fileName)
{	
	cat(connfd, fileName, IS_RETRIEVE);
}
