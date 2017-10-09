#include "tiny.h"
#include "rio.h"
#include "mySocket.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    while(1)
    {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (__CONST_SOCKADDR_ARG)&clientaddr, &clientlen);
        doit(connfd);
        close(connfd);
    }
}


