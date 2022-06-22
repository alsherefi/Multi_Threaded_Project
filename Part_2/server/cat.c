#include "project.h"
#define CAT_BUF 2097152
#define MAX 10000

void cat(int connectfd, char *filename, int32_t flags)
{
	uid_t uid = getuid(); /* Gets the user ID */
	uid_t euid = geteuid(); /* Gets the effective user ID */
	struct dirent *file_data;
	DIR *directory; /* Name of directory */
	char *fname; /* Name of the file that i will read from the directory */
	//char buffer; /* Storing the text of the file */
	char *buf1, *buf2;
	//int32_t size;
	int size;
	struct stat st;
	int buffer_position = 0;
	int counter = 0, star_index; /* Counter to know if read any file or not, star_index to know if string has * or not */
	int32_t flag_checker = flags & IS_RETRIEVE; // Check if we want to cat or retrieve.

	/* chroot */
	if (uid == 0 && uid == euid) { // chroot 
		if (chroot(".") < 0)
			err("chroot Error");
	} else
		err("User ID error");
	directory = opendir(".");
	if (directory == NULL)
		err("Error opening directory");


	buf1 = calloc(CAT_BUF, 1);

	while ((file_data = readdir(directory)) != NULL) { /* Reading the directory */

		fname = file_data->d_name;
		star_index = check_star(filename);

		if (star_index != -1) { /* Case if string has star */

			// l*c. the first part checks for l and the second part checks for c.
			if (strcmp_reverse(fname, filename + star_index + 1) == 0 && strncmp(fname, filename, star_index) == 0) {

				int fd = open(fname, O_RDONLY);

				if (fd < 0)
					err("Open Failed!");

				if (fstat(fd, &st) < 0)
					err("Error getting size");

				size = st.st_size; 
				
				// If we are retrieving send the file name to the client.	
				if (flag_checker == IS_RETRIEVE) {
					++retrieved_files;
					send_reply(connectfd, fname, flags | IS_NEWFILE | IS_CHUNK);
				}
				// Store the data in chunks of block size.
				while (size > st.st_blksize) {
					if (readn(fd, buf1 + buffer_position, st.st_blksize) < 0)
						err("Error reading from the file");
					size -= st.st_blksize;
					buffer_position += st.st_blksize;

					// If the buffer is about to be full. send the buffer to the client and reset it.
					// This block is used for huge files.
					if (buffer_position + st.st_blksize >= CAT_BUF) {
						send_reply(connectfd, buf1, flags | IS_CHUNK);
						buffer_position = 0;
						bzero(buf1, CAT_BUF);
					}
				}

				// Store the last data in the buffer and send it to the client.
				if (readn(fd, buf1 + buffer_position, size) < 0)
					err("Error reading from the file");

				send_reply(connectfd, buf1, flags | IS_CHUNK | IS_LAST_CHUNK);

				if (close(fd) < 0)
					err("Closing file error");

				counter++;
				bzero(buf1, CAT_BUF);
				buffer_position = 0;
			}
		} else if (strcmp(fname, filename) == 0) { /* Case if string named explicitly */
			int fd = open(fname, O_RDONLY);

			if (fd < 0)
				err("Open Failed!");

			if (fstat(fd, &st) < 0)
				err("Error getting size");

			size = st.st_size;

			// If we are retrieving send the file name to the client.	
			if (flag_checker == IS_RETRIEVE) {
				++retrieved_files;
				send_reply(connectfd, fname, flags | IS_NEWFILE | IS_CHUNK);
			}
			// Store the data in chunks of block size.
			while (size > st.st_blksize) {
				if (readn(fd, buf1 + buffer_position, st.st_blksize) < 0) 
					err("Error reading from the file");

				size -= st.st_blksize;
				buffer_position += st.st_blksize;

				// If the buffer is about to be full. send the buffer to the client and reset it.
				// This block is used for huge files.
				if (buffer_position + st.st_blksize >= CAT_BUF) {
					send_reply(connectfd, buf1, flags | IS_CHUNK);
					buffer_position = 0;
					bzero(buf1, CAT_BUF);
				}
			}

			// Store the last data in the buffer and send it to the client.
			if (readn(fd, buf1 + buffer_position, size) < 0)
				err("Error reading from the file");

			send_reply(connectfd, buf1, flags | IS_LAST_CHUNK);

			if (close(fd) < 0)
				err("Closing file error ");

			free(buf1);
			return;
		}
	}

	closedir(directory);

	if (counter < 1) {
		sprintf(buf1, "Cannot access '%s': No such file or directory\n", filename);
		send_reply(connectfd, buf1, 0);

	// For wildcards only. If we sent all the *c files (for example), There is no more files. Exit the receive_reply function.
	} else if (counter > 1)
		send_reply(connectfd, NULL, IS_FINISHED);

	free(buf1);
}
