//SEREVER part

#include "project.h"

#define LIST_SIZE 8

// 5 global vars for stat
int bytes_sent = 0; 
int bytes_received = 0;
int clients_counter = 0;
int retrieved_files = 0;
struct timeval TIME ;

// frees the data
void clean_vars(void)
{
	if(cmd != NULL)
		free(cmd);
	
	cmd = NULL;
	ln = NULL;
}

//command interface. We are considiring multi-threading, so we made this function.
// It simply uses receive_cmd to receive command from the client. and than let the parse handles the string and prepare the data for send_reply.
void interface(int connfd){
	char none = '\0';

	printf("Connection successful\n");
	++clients_counter;
	while (1) {
		cmd = receive_cmd(connfd);
		cmd = strtok(cmd, " \n");
		printf("Command: '%s' is entered by the client\n", cmd);
		
		if(cmd == NULL){
			send_reply(connfd, "", 0);
			continue;
		}

		ln = strtok(NULL, "\n");

		skip_leading_spaces(&ln);

		if (ln == NULL)
			ln = &none;


		printf("Line: '%s' is entered by the client\n", ln);

		parse_cmd(connfd);

		if (strcmp("exit", cmd) == 0)
			return;

		clean_vars();
	}
}

void skip_leading_spaces(char ** strp)
{
	if(*strp == NULL || strp == NULL)
		return;

	char *str = *strp;

	for(;*str == ' ' && str != NULL; ++str);
	
	*strp = str;
}


void stats(int connfd) {
	char *buffer;
	int s;
	struct timeval currTime, passTime;
	if(gettimeofday(&currTime, NULL) < 0)
		err("ERORR: unable to get time in stat");

	if(gettimeofday(&passTime, NULL) < 0)
		err("ERORR: unable to get time in stat");

	timersub(&currTime, &TIME, &passTime);

	buffer = calloc(BYTES_BUFFER, 1);
	if (buffer == NULL)
		err("Calloc error in stats function");

	sprintf(buffer, "\nBytes sent: %d\n", bytes_sent);
	sprintf(buffer, "%sBytes received: %d\n",buffer, bytes_received);
	sprintf(buffer, "%sTotal clients connected: %d\n",buffer, clients_counter);
	sprintf(buffer, "%sRetrieved files: %d\n", buffer, retrieved_files);
	s = sprintf(buffer, "%sServer uptime: %ld.%04ld ms\n\n", buffer, (long int) passTime.tv_sec, (long int) passTime.tv_usec);

	printf("%s", buffer);
	send_reply(connfd, buffer, 0);
	free(buffer);
}

int main(int argc, char **argv)
{
	int sockfd, connfd;
	struct sockaddr_in addr, client_addr;
	int len;
	char buffer[30];
	int s; // buffer size;
	
	gettimeofday(&TIME, NULL); //save current time int TIME.

	len = sizeof(addr);

	if (argc < 2)
		err("Error: Add a port to argv");

	// sockaddr_in
	bzero(&addr, len);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(atoi(argv[1]));

	// Creating scoketfd for the server.
	sockfd = socket(addr.sin_family, SOCK_STREAM, 0);
	if (sockfd < 0)
		err("Server socket Error");

	if (bind(sockfd, (struct sockaddr *) &addr, len) < 0)
		err("Binding Error");

	if (listen(sockfd, 1024) < 0)
		err("Listen Error");
	
	cmd = NULL;
	ln = NULL;

	//signal handling.
	if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
		err("signal error : SISUNT");

	if (signal(SIGSEGV, handler_SIGSEGV) == SIG_ERR)
		err("signal error : SIGSEGV");

	printf("Listening...\n");
	while(1) {
		connfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *)&len);
		if(connfd < 0)
			err("Error connecting to client");

		interface(connfd);
	}

	

	printf("Disconneting\n");
	close(connfd);
	printf("\nsomthing is wrong, this part of code is unreachble!\nmain: after while loop.\n");
	return 0;

}


void parse_cmd(int connfd)
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

	char buff[50];
	bzero(buff, 50);

	//Parsing with the list
	int indx;

	for (indx = 0; indx < LIST_SIZE; ++indx) {
		if (strcmp(cmd, cmd_list[indx]) == 0)
			break;
	}

	//dont delete, its to help debugging when needed.
	//printf("\ncommand = [%s]\ncommand size = [%zu]\nline = [%s]\nline size = [%zu]\nindex = [%d]\n\n", cmd, cmd.size, ln, ln.size, indx);

	switch (indx) {
	case 0://help
		help(connfd);
		//printf("help\n");
		break;
	case 1://ls
		ls(connfd, ln);
		//printf("ls\n(%s)\n", line);
		break;
	case 2://cat
		//printf("cat\n(%s)\n", ln);
		cat(connfd, ln, 0);
		break;
	case 3://retreive
		printf("retreive\n");
		retrieve(connfd, ln);
		break;
	case 4://stats
		//printf("stats\n");
		stats(connfd);
		break;
	case 5://exit   closing connection
		printf("closing connection...\n");
		send_reply(connfd, "Disconnecting...\n", 0);
		close(connfd);
		//__exit();
		break;
	case 6://empty line
		send_reply(connfd, ">", 0);
		break;
	case 7://println
		printf("command : [%s]\nline : [%s]\n", cmd, ln);
		sprintf(buff, "command : [%s]\nline : [%s]\n", cmd, ln);
		send_reply(connfd, buff, 0);
		break;
	case 8:
		printf("this command ( %s ) is not found\n", cmd);
		sprintf(buff, "this command ( %s ) is not found\n", cmd);
		send_reply(connfd, buff, 0);
		break;
	default:
		printf("ERROR, ( %s )\nINDX = %d", cmd, indx);	//should not reach this!
	}
}
