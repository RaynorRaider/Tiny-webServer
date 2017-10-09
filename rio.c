#include <errno.h>
#include <unistd.h>
#include "rio.h"

 ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0)
    {
        if((nread = read(fd, bufp, nleft)) < 0)
        {
            if(errno == EINTR)  //被信号中断,再次调用read
                nread = 0;
            else
                return -1;
        }
        else if(nread == 0)     //EOF
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (n-nleft);
}

 ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while(nleft > 0)
    {
        if((nwritten = write(fd, bufp, nleft)) <= 0)
        {
            if(errno == EINTR)      //被信号中断,重新调用write
                nwritten = 0;
            else
                return -1;
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}

 void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

 ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;

    while(rp->rio_cnt <= 0)         //如果缓冲区空了则重新填满
    {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));

        if(rp->rio_cnt < 0)
        {
            if(errno != EINTR)      //被信号中断
                return -1;
        }
        else if(rp->rio_cnt == 0)   //EOF
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;   //重置指针,指向缓冲区头部
    }   

    //返回缓冲区中n个字符，若不足，则返回缓冲区中剩余所有字符
    cnt = rp->rio_cnt < n? rp->rio_cnt : n;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

 ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int n, rc;
    char c;
    char *bufp = usrbuf;

    for(n=1; n<maxlen; n++)
    {
        if((rc = rio_read(rp, &c, 1)) == 1)
        {
            *bufp++ = c;
            if(c == '\n')       //读到换行符就结束
                break;
        }
        else if(rc == 0)        //EOF
        {
            if(n == 1)
                return 0;       //没有读任何数据
            else
                break;          //读了一些数据
        }
        else                    //错误
        {
            return -1;
        }
    }
    *bufp = 0;
    return n;
}

 ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0)
    {
        if((nread = rio_read(rp, bufp, nleft)) < 0)
        {
            if(errno == EINTR)      //被信号中断,再次调用rio_read()
                nread = 0;
            else
                return -1;          //error
        }
        else if(nread == 0)         //EOF
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}
