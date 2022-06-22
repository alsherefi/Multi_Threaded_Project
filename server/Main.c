//SERVER part

#include "project.h"

//Mutex
pthread_mutex_t CLIENTS_FDS_MUTEX = PTHREAD_MUTEX_INITIALIZER;

// 5 global vars for stat
int bytes_sent;
int bytes_received;
int clients_counter;
int retrieved_files;
struct timeval TIME;


/*
 * This function is used to skip leading spaces from the command.
 * Example: "                 ls"
 * Returns: no value.
 * The argumnets are:
 * strp: a pointer to the command
 */
void skip_leading_spaces(char **strp)
{
	if (*strp == NULL || strp == NULL)
		return;

	char *str = *strp;

	for (; *str == ' ' && str != NULL; ++str)
		;
	*strp = str;
}

/*
 * client_fd_init() is used to initialize all the client_fds array to -1.
 * This makes it easier to know which file is open and which file is closed.
 * Example: When making a new connection to the client. The first index with value -1
 * is used for this connection.
 * Returns: no value.
 * Arguments: no arguments.
 */
void client_fd_init(void)
{
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
		clients_fds[i] = -1;
}

/*
 * client_fd_add is used to add the new accept fd to the array
 * Returns: 0 in succesion and -1 if an error is occured
 * It takes one argument:
 * myfd: the accept fd.
 */
int client_fd_add(int myfd)
{
	if (myfd < 3)
		return -1;

	int i;

	pthread_mutex_lock(&CLIENTS_FDS_MUTEX);

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients_fds[i] == -1) {
			clients_fds[i] = myfd;
			pthread_mutex_unlock(&CLIENTS_FDS_MUTEX);
			return 0;
		}
	}
	pthread_mutex_unlock(&CLIENTS_FDS_MUTEX);
	return -1;
}

/*
 * client_fd_rm() removes the closed accept fd from the client_fds array.
 * Returns: 0 in succesion and -1 if an error is occured.
 * It takes one argument:
 * myfd: the closed accept fd.
 */
int client_fd_rm(int myfd)
{
	if (myfd < 3)
		return -1;

	int i;

	pthread_mutex_lock(&CLIENTS_FDS_MUTEX);

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients_fds[i] == myfd) {
			clients_fds[i] = -2;
			pthread_mutex_unlock(&CLIENTS_FDS_MUTEX);
			return 0;
		}
	}
	pthread_mutex_unlock(&CLIENTS_FDS_MUTEX);
	return -1;
}


/* It simply uses receive_cmd to receive command from the client.
 * And than let the parse handles the string and prepare the data for send_reply.
 * Returns: no value.
 * It takes one argument:
 * connfd: the accept fd
 */
void interface(int connfd)
{
	char none = '\0';
	char *mycmd, *myln;
	char *saveptr;

	printf("Connection successful\n");
	pthread_mutex_lock(&clients_counter_mutex);
	++clients_counter;
	pthread_mutex_unlock(&clients_counter_mutex);
	while (1) {
		mycmd = receive_cmd(connfd);
		mycmd = strtok_r(mycmd, " \n", &saveptr);
		printf("Command: '%s' is entered by the client %d\n", mycmd, connfd);
		if (mycmd == NULL) {
			send_reply(connfd, "", 0, 0);
			continue;
		}

		myln = strtok_r(NULL, "\n", &saveptr);

		skip_leading_spaces(&myln);

		if (myln == NULL)
			myln = &none;


		printf("Line: '%s' is entered by the client %d\n", myln, connfd);

		parse_cmd(connfd, mycmd, myln);

		if (strcmp("exit", mycmd) == 0)
			return;

		if (mycmd != NULL)
			free(mycmd);

		mycmd = NULL;
		myln = NULL;
	}
}

/* This is the function that is used from pthread_create()
 * Returns: void pointer, but in our case: nothing.
 * It takes one argument:
 * void *vargp: It takes the accept fd which is an integer by default.
 */
void *thread(void *vargp)
{
	int connfd = *((int *)vargp);

	pthread_detach(pthread_self());
	free(vargp);
	interface(connfd);
	printf("Disconneting client %d\n", connfd);
	close(connfd);
	client_fd_rm(connfd);
	return 0;
}

/*
 * The stats function prints the stats in the server and sends the stats to the client.
 * The stats are: sent bytes to the client, received bytes from the client, total clients connected to the server-
 * , Total retrieved (downloaded) files, and the total time that the server is on.
 * Returns: no value.
 * This function takes one argument
 * connfd: the accept fd
 */
void stats(int connfd)
{
	char *buffer;
	int s;
	struct timeval currTime, passTime;

	if (gettimeofday(&currTime, NULL) < 0)
		err("unable to get time", __func__);

	if (gettimeofday(&passTime, NULL) < 0)
		err("unable to get time", __func__);

	timersub(&currTime, &TIME, &passTime);

	buffer = calloc(BYTES_BUFFER, 1);
	if (buffer == NULL)
		err("Calloc error", __func__);

	pthread_mutex_lock(&bytes_sent_mutex);
	sprintf(buffer, "\nBytes sent: %d\n", bytes_sent);
	pthread_mutex_unlock(&bytes_sent_mutex);

	pthread_mutex_lock(&bytes_received_mutex);
	sprintf(buffer, "%sBytes received: %d\n", buffer, bytes_received);
	pthread_mutex_unlock(&bytes_received_mutex);

	pthread_mutex_lock(&clients_counter_mutex);
	sprintf(buffer, "%sTotal clients connected: %d\n", buffer, clients_counter);
	pthread_mutex_unlock(&clients_counter_mutex);

	pthread_mutex_lock(&retrieved_files_mutex);
	sprintf(buffer, "%sRetrieved file: %d\n", buffer, retrieved_files);
	pthread_mutex_unlock(&retrieved_files_mutex);

	pthread_mutex_lock(&bytes_sent_mutex);
	s = sprintf(buffer, "%sServer uptime: %ld.%04ld ms\n\n", buffer, (long) passTime.tv_sec, (long) passTime.tv_usec);
	pthread_mutex_unlock(&bytes_sent_mutex);

	printf("%s", buffer);
	send_reply(connfd, buffer, s, 0);
	free(buffer);
}

/*
 * listen_init: will bind a connection and listen to clients.
 * Returns: the socket fd.
 * Arguments: port number as an string, but converted inside this function to an integer.
 */
int listen_init(char *port)
{
	int sockfd;
	struct sockaddr_in addr;

	int len = sizeof(addr);

	// sockaddr_in
	bzero(&addr, len);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(atoi(port));

	// Creating scoketfd for the server.
	sockfd = socket(addr.sin_family, SOCK_STREAM, 0);
	if (sockfd < 0)
		err("Server socket Error", __func__);

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		err("Binding Error", __func__);

	if (listen(sockfd, 1024) < 0)
		err("Listen Error", __func__);

	LISTENFD = sockfd;

	return sockfd;

}

//signal handler initializer
void sig_init(void)
{
	//signal handling.
	if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
		err("signal error : SISUNT", __func__);

	if (signal(SIGSEGV, handler_SIGSEGV) == SIG_ERR)
		err("signal error : SIGSEGV", __func__);
}

/*
 * clients_handler() will wait for connections from the clients and link every client with a new thread.
 * Returns: no value.
 * It takes one argument:
 * sockfd: the socket fd that listens to clients.
 */
void clients_handler(int sockfd)
{
	int *connfdp, len;
	struct sockaddr_in client_addr;
	pthread_t tid;

	while (1) {
		len = sizeof(struct sockaddr_in);
		connfdp = malloc(sizeof(int));
		*connfdp = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *)&len);
		if (client_fd_add(*connfdp) == -1)
			err("Error adding client to the clients_fds list", __func__);

		if (*connfdp < 0)
			err("Error connecting to client", __func__);

		pthread_create(&tid, NULL, thread, connfdp);
	}
}

/*
 * We simplified main and used it to call functions only.
 * The argument argv must have the port number in argv[1]
 */
int main(int argc, char **argv)
{
	int sockfd;

	bytes_sent = 0;
	bytes_received = 0;
	clients_counter = 0;
	retrieved_files = 0;
	if (argc < 2)
		err("Add a port to argv", __func__);
	sig_init();
	client_fd_init();
	sockfd = listen_init(argv[1]);
	printf("Listening...\n");
	gettimeofday(&TIME, NULL); //save current time int TIME.
	clients_handler(sockfd);

	//shouldn't reach this part
	printf("\n SOMTHING IS WRONG! REACHED DEAD SPOT.\n");
	return 0;
}


/*
 * parse_cmd is used to parse through the command and call the right function.
 * ls, cat, ret, help, or exit.
 * Returns: no value.
 * It takes three arguments:
 * connfd: the accept fd.
 * mycmd: the command that we will parse through ret, help ... etc.
 * myln: the line after the command. Example, ls *c *txt. '*c *txt' is myln.
 */
void parse_cmd(int connfd, char *mycmd, char *myln)
{
	char *cmd_list[LIST_SIZE] = {
		"help",		//0
		"ls",		//1
		"cat",		//2
		"ret",		//3
		"stats",	//4
		"exit",		//5
		"",		//6
		"println"	//7
	};

	char *buff;
	int bytes;
	//Parsing with the list
	int indx;

	buff = calloc(strlen(mycmd) + 50, 1);
	if (buff == NULL)
		err("buff Calloc error", __func__);

	for (indx = 0; indx < LIST_SIZE; ++indx) {
		if (strcmp(mycmd, cmd_list[indx]) == 0)
			break;
	}

	switch (indx) {
	case 0://help
		help(connfd);
		break;

	case 1://ls
		ls(connfd, myln);
		break;

	case 2://cat;
		cat(connfd, myln, 0);
		break;

	case 3://retreive
		retrieve(connfd, myln);
		break;

	case 4://stats
		stats(connfd);
		break;

	case 5://exit   closing connection
		printf("closing connection with client %d...\n", connfd);
		bytes = sprintf(buff, "Disconnecting...\n");
		send_reply(connfd, buff, bytes, 0);
		close(connfd);
		client_fd_rm(connfd);
		break;

	case 6://empty line
		send_reply(connfd, ">", 1, 0);
		break;

	case 7://println
		printf("command : [%s]\nline : [%s]\n", mycmd, myln);
		bytes = sprintf(buff, "command : [%s]\nline : [%s]\n", mycmd, myln);
		send_reply(connfd, buff, bytes, 0);
		break;

	case 8:
		printf("this command ( %s ) is not found\n", mycmd);
		bytes = sprintf(buff, "this command ( %s ) is not found\n", mycmd);
		send_reply(connfd, buff, bytes, 0);
		break;

	default:
		printf("ERROR, ( %s )\nINDX = %d", mycmd, indx);	//should not reach this!
	}
	free(buff);
}
