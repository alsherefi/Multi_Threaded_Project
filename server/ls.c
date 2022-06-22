//Note for my teammates: use sudo ./program-name to be able to run the program.
#include "project.h"

/*
 * ls function is implelented from the ls commands from the shell
 * It handlers file names, wild cards. Examples (ls test.txt, ls *c, ls c*c, ls c*c a*b).
 * It takes only one wild card. ls c*c*c isn't possible.
 * Returns: no value.
 * It takes two arguments.
 * connfd: the accept fd.
 * ftype: the line of the command. *c *txt for example.
 */
void ls(int connfd, char *ftype)
{
	int star_index; // This variable stores the index of the "*" in ftype if there is one.
	int counter; // Counter to count the number of files found.
	char *fname; // fname used to store file_data->d_name
	char *token; // to parse through ftype.
	DIR *dar;
	char *saveptr;
	char *buffer;
	char *str_ftype = ftype; // the second time will convert it to null for strtok_r
	// readdir() function returns a struct that has the file data like the file name, file type ... etc.
	struct dirent *file_data;
	int bytes = 0;
	uid_t uid = getuid(); // Gets the user ID
	uid_t euid = geteuid(); // Gets the effective user ID

	// chroot
	if (uid == 0 && uid == euid) {
		if (chroot(".") < 0)
			err("chroot Error", __func__);
	} else
		err("User ID error", __func__);

	buffer = calloc(LS_BUFFER, 1);
	if (buffer == NULL)
		err("Error in allocating buffer", __func__);

	// Case 1: this case to print all the files
	if (ftype == NULL || ftype[0] == '\0') {

		dar = opendir(".");
		if (dar == NULL)
			err("Error opening directory in line 26", __func__);

		while ((file_data = readdir(dar)) != NULL) {
			fname = file_data->d_name;
			if (fname[0] != '.') // to avoid hidden files.
				bytes = sprintf(buffer, "%s%s\n", buffer, fname);
		}

		closedir(dar);

		send_reply(connfd, buffer, bytes, 0);
		free(buffer);
		return;
	}

	// Case 2 and 3: for the wildcards and filenames (can take more than one. Example: ls *txt test.c he*c).
	while ((token = strtok_r(str_ftype, " \n", &saveptr)) != NULL) {
		str_ftype = NULL;
		dar = opendir(".");// This will open a directory of type DIR*

		if (dar == NULL)
			err("Error opening directory", __func__);

		counter = 0;
		star_index = check_star(token);
		if (star_index != -1) {
			// store the next value in file_data variable until there is no files left.
			while ((file_data = readdir(dar)) != NULL) {

				fname = file_data->d_name;
				// strcmp_reverse will compare the characters after the "*". strncmp to compare the characters before the "*".
				if (fname[0] != '.' && strcmp_reverse(fname, token + star_index + 1) == 0 && strncmp(fname, token, star_index) == 0) {
					bytes = sprintf(buffer, "%s%s\n", buffer, fname);
					++counter;
				}
			}
		} else
			while ((file_data = readdir(dar)) != NULL) {

				fname = file_data->d_name;
				if (strcmp(fname, token) == 0) {// Check for file name, works even for hidden files.
					bytes += sprintf(buffer, "%s%s\n", buffer, fname);
					++counter;
				}

			}

		closedir(dar);

		if (counter == 0) {
			if (buffer == NULL)
				err("Error reallocating buffer when counter == 0", __func__);

				bytes = sprintf(buffer, "%sCannot access '%s': %s\n", buffer, token, strerror(ENOENT));
		}

	}

	send_reply(connfd, buffer, bytes, 0);
	free(buffer);
}


/*
 * chec_star is used  to check whether a wild card exist in the string or not.
 * Returns: the counter index in the array if it exists, -1 otherwise.
 * it takes one argument:
 * str: the string that we want to see if it has wildcard or not.
 */
int check_star(char *str)
{
	int counter = 0;

	while (str[counter] != '\0') {
		if (str[counter] == '*')
			break;

		++counter;
	}

	if (str[counter] == '\0')
		return -1;
	return counter;
}

/*
 * strcmp_reverse checkes the last characters between two strings.
 * Returns: 0 if the second string is empty
 * the result of strcmp between the last characters between the two strings.
 * It takes two arguments
 * s1: The first string.
 * s2: The second string which is usually the type (txt, c, exe).
 */
int strcmp_reverse(char *s1, char *s2)
{
	if (s2[0] == '\0')// '\0' exists in all of the strings.
		return 0;

	int s1_length = strlen(s1);
	int s2_length = strlen(s2);

	while (s1_length-- > s2_length && *s1 != '\0')
		++s1;

	return strcmp(s1, s2);
}

/*
 * The err function is a simple error checker
 * Returns: no value.
 * It takes one argument.
 * msg: a message to descripe the error
 */
void err(char *msg, const char *fname)
{
	fprintf(stderr, "%s: %s: %s\n", fname, msg, errno == 0 ? "\b\b " : strerror(errno));
	__exit();
}
