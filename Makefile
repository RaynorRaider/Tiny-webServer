all:	tiny

CC=gcc

tiny:	main.c tiny.o rio.o mySocket.o tiny.h rio.h mySocket.h
	$(CC) main.c tiny.o rio.o mySocket.o -lpthread -o tiny

tiny.o:	tiny.c tiny.h
	$(CC) -c tiny.c -o tiny.o

rio.o:	rio.c rio.h
	$(CC) -c rio.c -o rio.o

mySocket.o:	mySocket.c mySocket.h
	$(CC) -c mySocket.c -o mySocket.o

# two programs to test mySocket lib
sockTest_cli:	mySocket.o mySocket.h sockTest_cli.c
	$(CC) mySocket.o sockTest_cli.c -o sockTest_cli

sockTest_ser:	mySocket.o mySocket.h sockTest_ser.c
	$(CC) mySocket.o sockTest_ser.c -o sockTest_ser

#clean files
clean:
	rm -f SockTest_cli SockTest_ser *.o
