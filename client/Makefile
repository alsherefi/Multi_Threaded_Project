all : Client

Client : client.o handler.o read_and_write.o exit.o
	gcc -o Client client.o handler.o read_and_write.o exit.o

client.o : client.c
	gcc -c -g client.c

handler.o : handler.c
	gcc -c -g handler.c

read_and_write.o : read_and_write.c
	gcc -c -g read_and_write.c

exit.o : exit.c
	gcc -c -g exit.c

clean :
	rm *o
	rm Client
