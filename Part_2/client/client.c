#include "client.h"

int g_sockfd = -1;
char *buff = NULL;

//This function sends command to the server. First it sends the size of the buffer, and then it sends the buffer.
void send_cmd(int socketfd, char *buf)
{
	//remove leading spaces.
	char *cmd = skip_leading_spaces(buf);

	// Dr. Naser recomended me to use the int32_t because some machines takes integers as 2 bytes.
	int32_t size = (int32_t) strlen(cmd) + 1;


	// Here we are sending the size of the buffer to the server.
	if (writen(socketfd, &size, sizeof(int32_t)) < 0)
		err("send_cmd: taking the size error");

	// Here we are sending the buffer to the client.
	if (writen(socketfd, cmd, size) < 0)
		err("send_cmd: Error sending the buffer to the server");

}

void receive_reply(int socketfd)
{
	char *result; // Storing the result here
	int size; // The size of the result buffer.
	char *str; // str is used to parse through the buffer
	int32_t flags; // Flags are made to know whether we are dealing with retrieve or anything else.
	int32_t flag_checker; // I noticed bitwise operators work differently inside branches, so I store it here first.
	int fd = 1;// if it's retrieve not retrieve, This value won't be changed and the result will be printed to the output.
	
	do {

		// Gets the flags from the server
		if (readn(socketfd, &flags, sizeof(int32_t)) < 0)
			err("receive_reply: Error receiving the first flag");

		// If cat and retrieve used wildcard, This if statement will let the code know that we read all the files. Get out of the function.
		if (flags == IS_FINISHED)
			return;

		// Get the size from the server.
		if (readn(socketfd, &size, sizeof(int32_t)) < 0)
			err("Error reading total bytes in receive_reply");

		result = calloc(size, 1);

		if (result == NULL)
			err("Error in caloc in receive reply function");

		str = result;

		// For retrieve only. We check if it's a new file, or an old file but sent in chunks
		flag_checker = flags & IS_NEWFILE;
		if (flag_checker == IS_NEWFILE) {
			
			if (readn(socketfd, str, size) < 0)
				err("receive_reply: Error reading the file name");

			fd = open(str, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);	
			if (fd < 0)
				err("receive_reply: Error opening file");
			printf("Donwloading %s... ", str);
			continue;
		}
			

		// Read the data from the server, and write the data in the fd
		// fd (stdout if not retrive. Into a file if retrieve)
		while (size > MAX) {
			if (readn(socketfd, str, MAX) < 0)
				err("Error reading data from receive_reply function");
			if (writen(fd, str, MAX) < 0)
				err("receive_reply: Error writing data to file descriptor");

			str += MAX;
			size -= MAX;
		}


		// Reads and Writes the last bytes left.
		if (readn(socketfd, str, size) < 0)
			err("recieve_reply: Error reading the last bytes");

		if (writen(fd, str, size) < 0)
			err("recieve_reply: Error writing the last bytes");

		free(result);

		// retrieve only: Checks if the file is finished or there is another data that needs to be sent
		// If it's the last chunk close the file.
		flag_checker = flags & IS_LAST_CHUNK;
		if (fd != 1 && flag_checker == IS_LAST_CHUNK) {
			if (close(fd) < 0)
				err("receive_reply: Error closing file");
			fd = 1;
			printf("Done.\n");
		}
	} while ((flag_checker = flags & IS_CHUNK) == IS_CHUNK);

}


char* skip_leading_spaces(char *strp)
{
	if(strp == NULL)
		return strp;

	char *str = strp;

	for(;*str == ' ' && str != NULL; ++str);
	
	return str;
}


int main(int argc, char **argv)
{

	if (argc < 2)
		err("main: Error, please enter a port as an argument");

	//signal handling.
        if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
                err("signal error : SIGINT");

        if (signal(SIGSEGV, handler_SIGSEGV) == SIG_ERR)
                err("signal error : SIGSEGV");
	
	//declarations
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(atoi(argv[1]));
	
	int socketfd;
	char *message, *server_reply;
	size_t size = SIZE;

	buff = (char *) calloc(SIZE, sizeof(char));
	check_malloc(buff);

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("Creating socketfd failed!");

	if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) < 0)
		err("Connecting failed!");

	g_sockfd = socketfd;

	while(1) {
		if (write(1, "> ", 2) < 0)
			err("ERORR: unable to print the > in client main");

		getline(&buff, &size, stdin);

		//if (strcmp(buff, "\n") == 0)
		//	continue;

		send_cmd(socketfd, buff);
		receive_reply(socketfd);

		if (strcmp(buff, "exit\n") == 0)
			__exit();

		bzero(buff, SIZE);
	}
	return 0;
}
