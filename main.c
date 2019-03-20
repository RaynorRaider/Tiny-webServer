#include "tiny.h"
#include "rio.h"
#include "mySocket.h"
#include <stdio.h>
#include <pthread.h>

void * create_worker(void * args);

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
        pthread_t pid = 0;
        int fd = connfd;
        void *args = &fd;
        pthread_create(&pid, NULL, create_worker, (void *)args);
        pthread_detach(pid);
//        doit(connfd);
//        close(connfd);
    }
    return 0;
}

void *create_worker(void * args)
{
    int *pConnfd = (int *)args;
    doit(*pConnfd);
    close(*pConnfd);
    pthread_exit("thread exit sucessfully");
}

