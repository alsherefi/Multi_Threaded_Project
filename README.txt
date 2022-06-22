Project br-alwaldain

Team members:
Name: Abdullah Al-Osaimi, ID: 2172131914
Name: Abdulrahman Alsherefi, ID: 2171113977
Name: Omar Alsulaiteen, ID: 2131110461

Instructors
Dr. Naser Alduaij
Mr. Amro Attia

*****************************************************Brief explanation************************************************************

This project is about having a simple shell program that run some commands as the real shell.
We upgrade this shell to be run in a server and having the ability to have a client that can connect the this server and run the commands that been supported in the program.
After this we made the project to have the ability of having multiple clients using this shell concurrently at the same time instead of the ability to have only one client that can run this program.

*****************************************************Files************************************************************
Files in the Github:
        README.txt
        README1COPY.txt
        a.out
        alice29.txt
        asyoulik.txt
        checkpatch.pl
        counting
        cp.html
        fields.c
        filedscopy.c
        grammar.lsp
        hello
        lcet10.txt
        lib.a
        lotr.txt
        plrabn12.txt
        xargs.1

Directories in the Github:
        part1:                  The work that we did in phase 1 of the project.
        Part_2:                 The work that we did in phase 2 of the project.
        server:                 Explained below.
        client:                 Explained below.

client:
        Makefile:               to compile easily.
        client.c:               it will send the comand to server and will receive the reply from the server
                                and print the result.
                                it contains the client side functions like send_cmd(), recieve_reply(),
                                skip_leading_spaces(), connect_init(), user_handler() and the main function.

        exit.c:                 to exit from the program.
        read_and_write.c:       contains readn and writen functions to read and write the data.
        project.h:              contains the header and global vars, and the preprocessors.
        read_and_write.h:       contains header files and prototype of function readn and writen.
        flags.h:                contains flags that will help us in the project.
        handler.c:              contains handler functions to handel errors.

server:
        cat.c:                  print the content of each file in the directory on the client using send_reply()
                                function and deals with retrieve function to download the file or files.
                                it contains file_to_buffer(), check_star(), access(), strcmp_reverse()
                                , readn() and strcmp().

        flags.h:                it contains flags that will help us in the project.

        exit.c:                 to exit from the program.

        ls.c:                   to print the content of the directory on the client by sending the names of
                                the files in the directory to the client using send_reply() function.
                                it contains check_star() function, strcmp(), strcmp_reverse(), strncmp()
                                and err() function to handle errors.

        read_and_write.c:       to read from the client and write to the client by using readn and writen functions.
        read_and_write.h:       contains header files and prototype of function readn and writen.

        server_functions.c:     to do the tasks of the server that receive the command from the client
                                and send the reply to the client.
                                it contains the server side functions such as receive_cmd() and send_reply() and retrieve.
        server_functions.h:     contains the header files, global vars and preprocessors for the server.
        Main.c:                 it will handle the commands that comming from the client and sending
                                the reply to the client to do the proper task.

                                it contains these functions: skip_leading_spaces(), client_fd_init()
                                , client_fd_add(), client_fd_rm, interface(), thread(), stats()
                                , parse_cmd(), clients_handler(), sig_init(), listen_init() and the main function.

        makefile:               to compile easily.
        lib.a:                  the static library.
        lib.so:                 the shared library.
        handler.c:              contains handler function.
        help.c:                 to print help menu.

*****************************************************makefile*************************************************************
We also included a makefile to make compiling files easier. We included a static library lib.a and a shared library lib.so.

************************************************How to run the code****************************************************
How to run the code ?
simply follow the steps in terminal 1 (server):
1) make
2) sudo ./main <port number>

follow the steps in terminal 2 (client):
1) make
2) ./Client <port number>
3) enter the commands you want to execute.
