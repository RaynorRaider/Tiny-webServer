#ifndef RIO_H
#define RIO_H

#include<unistd.h>

#define RIO_BUFSIZE 8192
typedef struct{
    int rio_fd;                 //内部缓冲区的描述符
    int rio_cnt;                //缓冲区未读字符数
    char *rio_bufptr;           //缓冲区内下一个未读字符
    char rio_buf[RIO_BUFSIZE];  //内部缓冲区
}rio_t;

//无缓冲
 ssize_t rio_readn(int fd, void *usrbuf, size_t n);
 ssize_t rio_writen(int fd, void *usrbuf, size_t n);
//带缓冲
 void rio_readinitb(rio_t *rp, int fd);

 ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
 ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);

#endif

