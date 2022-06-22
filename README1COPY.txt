Project br-alwaldain

Team members:
Abdullah Al-Osaimi
Abdulrahman Alsherefi
Omar Alsulaiteen

Instructors
Dr. Naser Alduaij
Mr. Amro Attia

*************************************************Introduction*********************************************************
Introduction

Phase 1: Our project is a simple implementation of some of the shell commands

*****************************************************ls.c**************************************************************
ls.c: It contains: void ls(char *), int check_star(char *), int strcmp_reverse(char *, char *), and void err(char *):

a) ls(char*):
	Our ls function will be able to print the file names in the current directory, or a file name with a given name, or all of the
file names of some time if a wild card character is the first character of the parameter.

Examples:
	ls
	ls *txt
	ls test.c
	ls *txt *c test.c

b) int check_star(char *)
	This function will check whether the string contains a wild card character or not. If it does, return its index. Otherwise, return -1.

c) int strcmp_reverse(char *, char *):
	This function simply compares the last characters between two strings. Returns 0 if the last characters are equal, or the second string is empty.

d) void err(char *):
	Simple error handler.

******************************************************cat.c****************************************************************
cat.c: it contains: void cat(char *)

	By using this function, we can open the file (or files, if "*" is used) and read all the content and print it to the screen and if the called file doesn't exist, it will display an error.

Examples:
cat help.c
cat *.c
cat Main.c 

******************************************************Main.c***************************************************************

Main.c: it contains: void main(int, int), void parse_cmd(void),void user_input(void), void clean_vars(void):
It has two parts main and parse_cmd 

main:
	It takes input and splits it into both a command and a line using a function called user_input.

parse_cmd:
	It uses the command from main and parses it, to know which function to call. Then if needed it will pass the line from 
main function to the called function.

*****************************************************makefile*************************************************************
We also included a makefile to make compiling files easier. We included a static library lib.a. 

************************************************How to run the code****************************************************
How to run the code ?
simply follow the steps:
1) make
2) sudo ./main
3) enter the command you want to test. For example: cat test.c

alternatively, you can use 
gcc -c Main.c
gcc -o file_name Main.o ./lib.a
sudo ./file_name
