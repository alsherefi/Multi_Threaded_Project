#include "server_functions.h"
//these mutex locks to be used in stats function.
pthread_mutex_t bytes_received_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bytes_sent_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t retrieved_files_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * this function is return the command and take an argument which is connfd which it is the client file descriptor.
 * it will receive the command from the client and the command size.
 */
char *receive_cmd(int connfd)
{

	int bytes;
	char *command;
	int32_t size;

	//read the buffer size from the client.
	if (readn(connfd, &size, sizeof(int32_t)) < 0)
		err("Error in reading the command size", __func__);
	command = calloc(size, 1);

	//check for calloc
	if (command == NULL)
		err("Caloc error in command", __func__);

	//read the command from the client
	if (readn(connfd, command, size) < 0)
		err("Error reading command", __func__);

	//locking the bytes_received because it is a global variable.
	pthread_mutex_lock(&bytes_received_mutex);
	bytes_received += size + sizeof(int32_t);
	pthread_mutex_unlock(&bytes_received_mutex);
	return command;

}

/*
 * this function will send the flag, buffer size and the buffer itself to the client.
 * this function has no return value and takes the arguments:
 * connfd which is the client fd.
 * str which is the buffer
 * len which is the size of the buffer.
 * flags to know if we are sending the data as chunks, if the case is retrieve or cat ...etc.
 */
void send_reply(int connfd, char *str, int32_t len, int32_t flags)
{
	int bytes;

	// Sending flag
	if (writen(connfd, &flags, sizeof(int32_t)) < 0)
		err("Error writing flags", __func__);

	// to let the client know if the case of using wildcard is finished (in cat only).
	if (flags == IS_FINISHED)
		return;

	bytes = len;

	// Sending the file size to the client.
	if (writen(connfd, &len, sizeof(int32_t)) < 0)
		err("Error sending the file size", __func__);

	//Sending every 1024 bytes(for now) to the client.
	while (len > BYTES_BUFFER) {
		if (writen(connfd, str, BYTES_BUFFER) < 0)
			err("Error sending string", __func__);
		str += BYTES_BUFFER;
		len -= BYTES_BUFFER;
	}

	// Sending the last bytes.
	if (writen(connfd, str, len) < 0)
		err("Error sending number of the last bytes", __func__);

	// locking the bytes_sent because it is global variable.
	pthread_mutex_lock(&bytes_sent_mutex);
	bytes_sent += bytes + sizeof(int);
	pthread_mutex_unlock(&bytes_sent_mutex);
}

/*
 * this function uses cat function to know if the case is retrieve.
 * this function has no return value and has these arguments:
 * connfd: client fd.
 * fileName: the file thatb will be downloaded.
 */
void retrieve(int connfd, char *fileName)
{
	cat(connfd, fileName, IS_RETRIEVE);
}
