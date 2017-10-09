#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

int open_clientfd(char *hostname, int port);
int open_listenfd(int port);

#endif
