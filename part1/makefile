all : main

main : Main.o lib.a
	gcc -o main Main.o ./lib.a

Main.o : Main.c
	gcc -c -g Main.c

lib.a : ls.o cat.o handler.o help.o exit.o
	ar rcs lib.a ls.o cat.o handler.o help.o exit.o

ls.o : ls.c
	gcc -c ls.c

handler.o : handler.c
	gcc -c handler.c

help.o : help.c
	gcc -c help.c

exit.o : exit.c
	gcc -c exit.c

cat.o : cat.c
	gcc -c cat.c

clean:
	rm *o main
