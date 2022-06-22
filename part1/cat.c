#include "project.h"
void cat(char *filename)
{

	uid_t uid = getuid(); /* Gets the user ID */
	uid_t euid = geteuid(); /* Gets the effective user ID */
	struct dirent *file_data;
	DIR *directory; /* Name of directory */
	char *fname; /* Name of the file that i will read from the directory */
	char buffer; /* Storing the text of the file */
	int counter = 0, star_index; /* Counter to know if read any file or not, star_index to know if string has * or not */
	/* chroot */
	if (uid == 0 && uid == euid) { /* chroot */
		if (chroot(".") < 0)
			err("chroot Error");
	} else
		err("User ID error");
	directory = opendir(".");
	if (directory == NULL)
		err("Error opening directory");

	while ((file_data = readdir(directory)) != NULL) { /* Reading the directory */
		fname = file_data->d_name;
		star_index = check_star(filename);
		if (star_index != -1) { /* Case if string has star */
			if (strcmp_reverse(fname, filename + star_index + 1) == 0 && strncmp(fname, filename, star_index) == 0) {
				int fd = open(fname, O_RDONLY);

				if (fd < 0)
					err("Open Failed!");

				while (read(fd, &buffer, sizeof(char)) > 0)
					write(1, &buffer, sizeof(char));

				printf("\n");
				if (close(fd) < 0)
					err("Closing file error");
				counter++;
			}
		} else if (strcmp(fname, filename) == 0) { /* Case if string named explicitly */
			int fd = open(fname, O_RDONLY);

			if (fd < 0)
				err("Open Failed!");

			while (read(fd, &buffer, sizeof(char)) > 0)
				write(1, &buffer, sizeof(char));

			printf("\n");
			if (close(fd) < 0)
				err("Closing file error ");
			counter++;
		}
	}
	closedir(directory);
	if (counter == 0)
		printf("No such file or directory\n");
}
