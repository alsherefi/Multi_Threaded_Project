//alo

#include "user_input.h"

void  user_input(char **mystr)			//cmd = the first word of user input, ln = the rest of user input.
{
	char ch;
	char flag = 1;			//the flags are char to save memory.
	char skipSpaceFlag = 1;		//to skip the spaces in front
	char isLineFlag = 0;		// to detect first word (command)( meaning after it reads the first word, not leading spacec).

	while (flag) {
		ch = getchar();
		if (ch == ' ' && skipSpaceFlag) {	//skip leading sapces.
			continue;
		} else {
			skipSpaceFlag = 0;
		}
		if (ch == '\n')
			ch = '\0';

		//check if we are saving ch in ln or cmd.
		if (isLineFlag) {

			ln.size++;
			ln.A = (char *)realloc(ln.A, ln.size * sizeof(int));
			check_malloc(ln.A);
			ln.A[ln.size - 1] = ch;
		} else {
			if (ch == ' ') {
				isLineFlag++;
				skipSpaceFlag = 1;
				continue;
			}

			cmd.size++;
			cmd.A = (char *)realloc(cmd.A, cmd.size * sizeof(int));
			check_malloc(cmd.A);
			cmd.A[cmd.size - 1] = ch;
		}

		if (ch == '\n' || ch == EOF || ch == '\0')
			flag = 0;

	}

	//dont delete, its to help debugging when needed.
	printf("\ncommand = [%s]\ncommand size = [%zu]\nline = [%s]\nline size = [%zu]\nflag = [%d]\nskipSpaceFlag = [%d]\n isLineFlag = [%d]\n\n", cmd.A, cmd.size, ln.A, ln.size, flag, skipSpaceFlag, isLineFlag);

	(*mystr) = strcat(cmd.A,":\0" );	
	(*mystr) = strcat(cmd.A, ln.A);

}
/*
void  user_input(void)			//cmd = the first word of user input, ln = the rest of user input.
{
	char ch;
	char flag = 1;			//the flags are char to save memory.
	char skipSpaceFlag = 1;		//to skip the spaces in front
	char isLineFlag = 0;		// to detect first word (command)( meaning after it reads the first word, not leading spacec).

	while (flag) {
		ch = getchar();
		if (ch == ' ' && skipSpaceFlag) {	//skip leading sapces.
			continue;
		} else {
			skipSpaceFlag = 0;
		}
		if (ch == '\n')
			ch = '\0';

		//check if we are saving ch in ln or cmd.
		if (isLineFlag) {

			ln.size++;
			ln.A = (char *)realloc(ln.A, ln.size * sizeof(int));
			check_malloc(ln.A);
			ln.A[ln.size - 1] = ch;
		} else {
			if (ch == ' ') {
				isLineFlag++;
				skipSpaceFlag = 1;
				continue;
			}

			cmd.size++;
			cmd.A = (char *)realloc(cmd.A, cmd.size * sizeof(int));
			check_malloc(cmd.A);
			cmd.A[cmd.size - 1] = ch;
		}

		if (ch == '\n' || ch == EOF || ch == '\0')
			flag = 0;

	}

	//dont delete, its to help debugging when needed.
	//printf("\ncommand = [%s]\ncommand size = [%zu]\nline = [%s]\nline size = [%zu]\nflag = [%d]\nskipSpaceFlag = [%d]\n isLineFlag = [%d]\n\n", cmd.A, cmd.size, ln.A, ln.size, flag, skipSpaceFlag, isLineFlag);



}
*/

int main(void)
{
	char *mystr = NULL;

	printf("please Enter: ");
	user_input(&mystr);

	printf("result: %s\n", mystr);

	free(cmd.A);
	free(ln.A);
	return 0;
}
