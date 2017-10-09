#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include"rio.h"
#include"mySocket.h"

#define MAXLINE 1024
void echo(int connfd);

int main(int argc, char **argv)
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    int haddrp;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    if(listenfd == -1)
    {
        fprintf(stderr, "can't open this port\n");
        exit(1);
    }
    while(1)
    {
        clientlen = sizeof(clientaddr);
        if((connfd = accept(listenfd, (__CONST_SOCKADDR_ARG)&clientaddr,
                        &clientlen)) == -1)
        {
            fprintf(stderr, "Something wrong in accept\n");
            exit(1);
        }
        else
        {
            hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                    sizeof(clientaddr.sin_addr.s_addr), AF_INET);

            haddrp = inet_ntoa(clientaddr.sin_addr);
            printf("server connected to %s (%d)\n", hp->h_name, haddrp);

            echo(connfd);
            close(connfd);
        }
    }
    close(listenfd);
    exit(0);
}

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    rio_readinitb(&rio, connfd);
    while((n = rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        printf("server received %d bytes\n", n);
        rio_writen(connfd, buf, n);
    }
}



