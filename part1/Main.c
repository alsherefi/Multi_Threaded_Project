//NAME: Abdullah Alosaimi, 2172131914

#include "project.h"
#define LIST_SIZE 8

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

void clean_vars(void)
{
	while (cmd.size > 0)
		cmd.A[--cmd.size] = '\0';

	while (ln.size > 0)
		ln.A[--ln.size] = '\0';

	cmd.size = 0;
	ln.size = 0;
}

int main(int argc, char **argv)
{
	clean_vars();
	cmd.A = NULL;
	ln.A = NULL;

	//signal handling.
	if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
		err("signal error : SISUNT");

	if (signal(SIGSEGV, handler_SIGSEGV) == SIG_ERR)
		err("signal error : SIGSEGV");


	//command interface
	while (1) {
		printf("%s >", argv[0]);
		user_input();
		parse_cmd();
		clean_vars();
	}

	printf("\nsomthing is wrong, this part of code is unreachble!\nmain: after while loop.\n");
	return 0;

}


void parse_cmd(void)
{
	char *cmd_list[LIST_SIZE] = {
		"help",		//0
		"ls",		//1
		"cat",		//2
		"ret",		//3
		"stat",		//4
		"exit",		//5
		"",		//6
		"println"	//7
	};

	//Parsing with the list
	int indx;

	for (indx = 0; indx < LIST_SIZE; ++indx) {
		if (strcmp(cmd.A, cmd_list[indx]) == 0)
			break;
	}

	//dont delete, its to help debugging when needed.
	//printf("\ncommand = [%s]\ncommand size = [%zu]\nline = [%s]\nline size = [%zu]\nindex = [%d]\n\n", cmd.A, cmd.size, ln.A, ln.size, indx);

	switch (indx) {
	case 0://help
		help();
		//printf("help\n");
		break;
	case 1://ls
		ls(ln.A);
		//printf("ls\n(%s)\n", line);
		break;
	case 2://cat
		//printf("cat\n(%s)\n", ln.A);
		cat(ln.A);
		break;
	case 3://retreive
		printf("retreive\n");
		break;
	case 4://stats
		printf("stats\n");
		break;
	case 5://exit
		printf("exiting...\n");
		__exit();
		break;
	case 6://empty line
		break;
	case 7://println
		printf("command : [%s]\nline : [%s]\n", cmd.A, ln.A);
		break;
	case 8:
		printf("this command ( %s ) is not found\n", cmd.A);
		break;
	default:
		printf("ERROR, ( %s )\nINDX = %d", cmd.A, indx);	//should not reach this!
	}
}
