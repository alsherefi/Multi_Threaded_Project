#include "project.h"

/*
 * file_to_buffer function opens a file, fills the data from the file to the buffer, and then send this buffer to the client. It also checks if it's retrieve or not.
 * The arguments are:
 * connectfd: client fd.
 * fname: the file name.
 * buf1: the buffer to be filled.
 * flags: retrieve or not retreive.
 * star_flag: Are we trying to read one file ? or multiple files? In other words: is it hello.c ? or *c ?
 */
void file_to_buffer(int connectfd, char *fname, char *buf1, int32_t flags, int32_t star_flag)
{
	int size; // The size of the file.
	int bytes; // bytes from sprintf
	struct stat st;
	int buffer_position = 0; // for parsing through buf1.
	int fd = open(fname, O_RDONLY);

	if (fd < 0)
		err("Open Failed!", __func__);

	if (fstat(fd, &st) < 0)
		err("Error getting size", __func__);

	//If the file is a directory, inform the client, and then continue the loop.
	if (S_ISDIR(st.st_mode)) {
		bytes = sprintf(buf1, "%s is a directory\n", fname);
		send_reply(connectfd, buf1, bytes, star_flag);
		return;
	}

	size = st.st_size;
	// If we are retrieving send the file name to the client.
	if ((flags & IS_RETRIEVE)) {
		pthread_mutex_lock(&retrieved_files_mutex);
		++retrieved_files;
		pthread_mutex_unlock(&retrieved_files_mutex);
		if ((st.st_mode & (0111))) // S_IXUSR | S_IXGRP | S_IXOTH
			send_reply(connectfd, fname, strlen(fname), flags | IS_NEWFILE | IS_CHUNK | IS_EXECUTE);
		else
			send_reply(connectfd, fname, strlen(fname), flags | IS_NEWFILE | IS_CHUNK);
	}
	// Store the data in chunks of block size.
	while (size > st.st_blksize) {
		if (readn(fd, buf1 + buffer_position, st.st_blksize) < 0)
			err("Error reading from the file", __func__);
		size -= st.st_blksize;
		buffer_position += st.st_blksize;

		// If the buffer is about to be full. send the buffer to the client and reset it.
		// This block is used for huge files.
		if (buffer_position + st.st_blksize >= CAT_BUF) {
			send_reply(connectfd, buf1, buffer_position, flags | IS_CHUNK);
			buffer_position = 0;
			bzero(buf1, CAT_BUF);
		}
	}

	// Store the last data in the buffer and send it to the client.
	if (readn(fd, buf1 + buffer_position, size) < 0)
		err("Error reading from the file", __func__);

	// Buffer position + size = total byte that needs to be sent.
	send_reply(connectfd, buf1, buffer_position + size, flags | star_flag | IS_LAST_CHUNK);

	if (close(fd) < 0)
		err("Closing file error", __func__);

	bzero(buf1, CAT_BUF);
	return;

}

/*
 * The cat function implements two functions. ret and cat.
 * The arguments are:
 * connectfd: the file the client fd.
 * filename:  The file name or a word with a wild card.
 * flags: if it equals 0 then we will cat, otherwise it's a retrieve.
 */
void cat(int connectfd, char *filename, int32_t flags)
{
	uid_t uid = getuid(); /* Gets the user ID */
	uid_t euid = geteuid(); /* Gets the effective user ID */
	struct dirent *file_data;
	DIR *directory; /* Name of directory */
	char *fname; /* Name of the file that i will read from the directory */
	//char buffer; /* Storing the text of the file */
	char *buf1;
	int bytes; // for sprintf
	int counter = 0, star_index; /* Counter to know if read any file or not, star_index to know if string has * or not */

	/* chroot */
	if (uid == 0 && uid == euid) {
		if (chroot(".") < 0)
			err("chroot Error", __func__);
	} else
		err("User ID error", __func__);

	// allocate the buffer and check for error.
	buf1 = calloc(CAT_BUF, 1);
	if (buf1 == NULL)
		err("Error in calloc \"buf1\"", __func__);

	// check_star to check if it's just a file name or there is a wild card included.
	star_index = check_star(filename);

	// If it's just a file name check if it exists using the access function, and then print or
	// download the file.
	if (star_index == -1) {
		if (access(filename, F_OK) == 0) {
			file_to_buffer(connectfd, filename, buf1, flags, 0);
			free(buf1);
			return;
		// ENOENT: file doesn't exist.
		} else if (errno == ENOENT) {
			bytes = sprintf(buf1, "Cannot access '%s': No such file or directory\n", filename);
			send_reply(connectfd, buf1, bytes, 0);
			free(buf1);
			return;
		}

		err("Error using access function", __func__);
	}

	// Open directory and check for error.
	directory = opendir(".");
	if (directory == NULL)
		err("Error opening directory", __func__);

	// addition to line 106. If it's not a file name and, we have a wild card included.
	//  Check for every file in the directory.
	while ((file_data = readdir(directory)) != NULL) { /* Reading the directory */

		fname = file_data->d_name;

		// l*c. the first part checks for l and the second part checks for c.
		if (strcmp_reverse(fname, filename + star_index + 1) == 0 && strncmp(fname, filename, star_index) == 0) {

			file_to_buffer(connectfd, fname, buf1, flags, IS_CHUNK);
			++counter;
		}
	}

	closedir(directory);

	// counter checks if at least one file was counted. If there is no file counted print the statment.
	if (counter < 1) {
		bytes = sprintf(buf1, "Cannot access '%s': No such file or directory\n", filename);
		send_reply(connectfd, buf1, bytes, 0);

	// If we sent all the *c files (for example), There is no more files. Exit the receive_reply function.
	} else
		send_reply(connectfd, NULL, 0, IS_FINISHED);

	free(buf1);
}
