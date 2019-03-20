#ifndef TINY_H
#define TINY_H

#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "rio.h"
#include "mySocket.h"

#define MAXLINE 1024
#define MAXBUF  4096

void doit(int fd);
int read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic_get(int fd, char *filename, char *cgiargs);
void serve_dynamic_post(int fd, char *filename, int bodyLength, rio_t *rp);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

#endif

