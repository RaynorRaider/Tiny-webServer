#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include "mySocket.h"

int open_clientfd(char *hostname, int port)
{
    int clientfd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;

    if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    //填写服务器IP地址及端口
    if((hp = gethostbyname(hostname)) == NULL)
        return -2;
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);

    //和服务器建立连接
    if(connect(clientfd, (__CONST_SOCKADDR_ARG)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    return clientfd;
}


int open_listenfd(int port)
{
    int listenfd;
    int optval = 1;
    struct sockaddr_in serveraddr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    //Eliminates "Address already in use" error from bind
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval, sizeof(int)) < 0)
        return -1;

    //接收该主机的任何IP地址
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if(bind(listenfd, (__CONST_SOCKADDR_ARG)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    //让接听端口准备监听
    if(listen(listenfd, 5) < 0)
        return -1;

     return listenfd;
}
