all : main

main : Main.c project.h lib.so
	gcc -g -o main Main.c -pthread ./lib.so

lib.so : ls.c cat.c handler.c help.c exit.c server_functions.c read_and_write.c flags.h project.h read_and_write.h server_functions.h
	 gcc -shared -fpic -o lib.so ls.c cat.c handler.c help.c exit.c server_functions.c read_and_write.c

clean:
	rm main lib.so
