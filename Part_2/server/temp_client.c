#include "project.h"
#define MAX 100
#define SIZE 4096

void send_cmd(int socketfd)
{
	size_t s = SIZE;
	int32_t size;
	char *cmd = calloc(SIZE, sizeof(char));

	write(1, "-> ", 3);
	size = getline(&cmd, &s, stdin);
	if (size == -1)
		err("Send cmd: error getting line from the user");

	++size;
	write(socketfd, &size, sizeof(int32_t));
	write(socketfd, cmd, size);
	bzero(cmd, SIZE);

}

void receive_reply(int socketfd, char *server_reply)
{
	char *result;
	int32_t size;
	char *str;

	if (read(socketfd, &size, sizeof(int32_t)) < 0)
		err("Error reading total bytes in receive_reply");

	result = calloc(size, 1);

	if (result == NULL)
		err("Error in caloc in receive reply function");

	str = result;
	while (size > MAX) {
		if (read(socketfd, str, MAX) < 0)
			err("Error reading data from receive_reply function");
		size -= MAX;
		str += MAX;
	}

	if (read(socketfd, str, size) < 0)
		err("recieve_reply: Error reading the last bytes");
	
	printf("%s", result);
	free(result);
}

int main(int argc, char **argv)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(atoi(argv[1]));
	int socketfd;
	char *message, *server_reply;

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Creating socketfd failed! %s\n", strerror(errno));
		exit(0);
	}
	if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Connecting failed! %s\n", strerror(errno));
		exit(0);
	}

	while(1) {
		send_cmd(socketfd);
		receive_reply(socketfd, server_reply);
	}
	return 0;
}
