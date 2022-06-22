#include "client.h"

int g_sockfd;
char *buff;

/*
 * This function sends command to the server. First it sends the size of the buffer, and then it sends the buffer.
 * This function has no return value and taking these arguments:
 * socketfd: the socket descriptor.
 * buf: the buffer that will be sent to the server.
 */
void send_cmd(int socketfd, char *buf)
{
	// remove leading spaces.
	char *cmd = skip_leading_spaces(buf);

	// Dr. Naser recomended me to use the int32_t because some machines take integers as 2 bytes.
	int32_t size = (int32_t) strlen(cmd) + 1;


	// Here we are sending the size of the buffer to the server.
	if (writen(socketfd, &size, sizeof(int32_t)) < 0)
		err("taking the size error", __func__);

	// Here we are sending the buffer to the client.
	if (writen(socketfd, cmd, size) < 0)
		err("Error sending the buffer to the server", __func__);

}

/*
 * this function receives the reply from the server and covers all the cases, ls, cat, ret, stats, help, exit.
 * this function has no retrun value and these are the arguments it take:
 * socketfd: socket file descriptor.
 */
void receive_reply(int socketfd)
{
	int file_copy_number = 0;
	int cond; // access function condition.
	char *result; // Storing the result here
	int size; // The size of the result buffer.
	char *str; // str is used to parse through the buffer
	int32_t flags; // Flags are made to know whether we are dealing with retrieve or anything else.
	int fd = 1; // if it's retrieve not retrieve, This value won't be changed and the result will be printed to the output.
	char fname[50]; // this takes the file name from the server for the retrieve case.

	do {

		// Gets the flags from the server
		if (readn(socketfd, &flags, sizeof(int32_t)) < 0)
			err("Error receiving the first flag", __func__);

		// If cat and retrieve used wildcard, This if statement will let the code know that we read all the files. Get out of the function.
		if (flags == IS_FINISHED)
			return;

		// Get the size from the server.
		if (readn(socketfd, &size, sizeof(int32_t)) < 0)
			err("Error reading total bytes", __func__);

		result = calloc(size, 1);

		if (result == NULL)
			err("Error in caloc", __func__);

		str = result;

		// For retrieve only. We check if it's a new file, or an old file but sent in chunks
		if ((flags & IS_NEWFILE)) {
			if (readn(socketfd, str, size) < 0)
				err("Error reading the file name", __func__);

			// I used the access function to check if the file already exists or not.
			// If it already exist. make a file_name(number). Just like how browsers do it.
			strncpy(fname, str, 50);
			while ((cond = access(fname, F_OK)) == 0)
				sprintf(fname, "copy_%d_%s", ++file_copy_number, str);

			// if there is an error (ENOENT: is not file or directory and this is not an error).
			if (cond == -1 && errno != ENOENT)
				err("error in access function", __func__);

			file_copy_number = 0;
			// if the file is executable or not.
			if ((flags & IS_EXECUTE))
				// 0770: S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP
				fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0770);
			else
				//0660: S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
				fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0660);

			if (fd < 0)
				err("Error opening file", __func__);
			printf("Downloading %s... ", fname);
			continue;
		}

		// Read the data from the server, and write the data in the fd
		// fd (stdout if not retrive. Into a file if retrieve)
		while (size > MAX) {
			if (readn(socketfd, str, MAX) < 0)
				err("Error reading data", __func__);
			if (writen(fd, str, MAX) < 0)
				err("Error writing data to file descriptor", __func__);

			str += MAX;
			size -= MAX;
		}


		// Reads and Writes the last bytes left.
		if (readn(socketfd, str, size) < 0)
			err("Error reading the last bytes", __func__);

		if (writen(fd, str, size) < 0)
			err("Error writing the last bytes", __func__);

		free(result);

		// retrieve only: Checks if the file is finished or there is another data that needs to be sent
		// If it's the last chunk close the file.
		if (fd != 1 && (flags & IS_LAST_CHUNK)) {
			if (close(fd) < 0)
				err("Error closing file", __func__);
			fd = 1;
			printf("Done.\n");
		}
	} while ((flags & IS_CHUNK));
}

/*
 * this function is removing the spaces between the line that inserted by the client.
 * this function has a return value which is the modified command and takes this argument:
 * strp: the command itself before being modified.
 */
char *skip_leading_spaces(char *strp)
{
	if (strp == NULL)
		return strp;
	char *str = strp;

	for (; *str == ' ' && str != NULL; ++str)
		;

	return str;
}

/*
 * This function is creating a socketfd and then connect to the server.
 * this function has a return value which is an integer and this integer is socketfd and the arguments that it takes are:
 * port: which is port number.
 */
int connect_init(char *port)
{

	//declarations
	int socketfd;
	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(atoi(port));

	buff = (char *) calloc(SIZE, sizeof(char));
	check_malloc(buff);

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
		err("Creating socketfd failed!", __func__);

	if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) < 0)
		err("Connecting failed!", __func__);

	g_sockfd = socketfd;
	return socketfd;
}

/*
 * this function initialise the signal handler.
 * this function has no return value and takes no arguments.
 */
void sig_init(void)
{

	//signal handling.
	if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
		err("signal error : SIGINT", __func__);

	if (signal(SIGSEGV, handler_SIGSEGV) == SIG_ERR)
		err("signal error : SIGSEGV", __func__);
}

/*
 * this function takes the command from the client and then send the result to the server and then receive the reply from the server.
 * this function has no return value and takes this argument:
 * socketfd: socket file descriptor.
 */
void user_handler(int socketfd)
{
	size_t size = SIZE;

	while (1) {
		if (write(1, "> ", 2) < 0)
			err("unable to print the > in client main", __func__);

		// the client takes the command from the user.
		getline(&buff, &size, stdin);

		// send the command to the server and then receive the reply from ther server.
		send_cmd(socketfd, buff);
		receive_reply(socketfd);

		// checking if the inserted string from the user was exit.
		if (strcmp(buff, "exit\n") == 0)
			__exit();

		bzero(buff, SIZE);
	}
}

/*
 * the main function is creating the socketfd and then call user_handler(socketfd).
 */
int main(int argc, char **argv)
{

	size_t size = SIZE;
	int socketfd;

	g_sockfd = -1;
	buff = NULL;

	if (argc < 2)
		err("please enter a port as an argument", __func__);
		//err("main: Error, please enter a port as an argument");

	socketfd = connect_init(argv[1]);

	user_handler(socketfd);
	printf("\nSOMTHING IS WRONG! THIS IS A DEAD PART OF THE CODE\n");
	return 0;
}
