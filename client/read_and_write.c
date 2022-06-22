#include "read_and_write.h"

/*
 * This function is reading the data from a given file and return the bytes that been read.
 * This function has a return value which is the bytes that been read and takes these arguments:
 * int fd: the file descriptor.
 * void *usrbuf: the buffer that will be filled in read case.
 * size_t n: total bytes that will be read from the fd.
 */
ssize_t readn(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;

	while (nleft > 0) {
		nread = read(fd, bufp, nleft);
		if (nread < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0)
			break;
		nleft -= nread;
		bufp += nread;
	}
	return n;
}

/*
 * This function is writing the data to a given file and return the bytes that been written.
 * This function has a return value which is the bytes that been written and takes these arguments:
 * int fd: the file descriptor.
 * void *usrbuf: the buffer that will take the data to write it on fd.
 * size_t n: total bytes that will be written on the fd.
 */
ssize_t writen(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = usrbuf;

	while (nleft > 0) {
		nwritten = write(fd, bufp, nleft);
		if (nwritten <= 0) {
			if (errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	return n;
}
