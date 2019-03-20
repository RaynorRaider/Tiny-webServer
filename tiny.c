#include "tiny.h"
#include <string.h>
#include <stdio.h>

extern char **environ;

void doit(int fd)
{
    int is_static;
    int body_length;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    //读取请求行及头部
    rio_readinitb(&rio, fd);
    rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);
    //仅支持GET, POST方法
    if(strcmp(method, "GET") && strcmp(method, "POST"))
    {
        clienterror(fd, method, "501", "Not Implemented", "Tiny does not implement this method");
        return;
    }

    body_length = read_requesthdrs(&rio);

    //从GET请求中解析URI
    is_static = parse_uri(uri, filename, cgiargs);
    if(stat(filename, &sbuf) < 0)
    {
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
        return;
    }

    if(is_static)       //提供静态文件
    {
        if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))   //是普通文件且有用户读权限
        {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size);
    }
    else                //提供动态文件
    {
        if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))   //是普通文件且有用户执行权限
        {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
            return;
        }

        if(strcmp(method, "GET") == 0)
        {
            serve_dynamic_get(fd, filename, cgiargs);
        }
        else if(strcmp(method, "POST") == 0)
        {
            serve_dynamic_post(fd, filename, body_length, &rio);
        }
    }
}

void clienterror(int fd, char *cause, char *errnum,
                char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    //创建HTTP响应主体
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    //打印HTTP响应
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Comtent-type: text/html\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, body, strlen(body));
}

//读取报头中的信息,返回Content-length的值
int read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];
    char *p = NULL;
    char len[10];
    rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n"))
    {
        if( p = strstr(buf, "Content-Length: "))
        {
            sprintf(len, "%s", p+sizeof("Content-Length:"));
            printf("len is : %s", len);
        }
        rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return atoi(len);
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    if(!strstr(uri, "cgi-bin"))     //uri中不包含cgi-bin,请求的是静态内容
    {
        strcpy(cgiargs, "");        //清除CGI参数

        //将uri转换为相对unix路径名
        strcpy(filename, ".");     
        strcat(filename, uri);
        //如果uri以'/'结尾，则补上默认文件名
        if(uri[strlen(uri)-1] == '/')
            strcat(filename, "home.html");
        return 1;
    }
    else        //请求的是动态内容
    {
        ptr = index(uri, '?');
        //抽取所有CGI参数
        if(ptr)
        {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else
            strcpy(cgiargs, "");
        
        //转换uri剩余部分为一个相对unix文件名
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}

void serve_static(int fd, char *filename, int filesize)
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    //发送响应报头给客户端
    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n", buf, filetype);
    rio_writen(fd, buf, strlen(buf));

    //发送响应主体给客户端
    srcfd = open(filename, O_RDONLY, 0);
    srcp = mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    close(srcfd);
    rio_writen(fd, srcp, filesize);
    munmap(srcp, filesize);
}

void get_filetype(char *filename, char *filetype)
{
    if(strstr(filename, ".html"))
    {
        strcpy(filetype, "text/html");
    }
    else if(strstr(filename, ".gif"))
    {
        strcpy(filetype, "image/gif");
    }
    else if(strstr(filename, ".jpg"))
    {
        strcpy(filetype, "image/jpeg");
    }
    else
    {
        strcpy(filetype, "text/plain");
    }
}

void serve_dynamic_get(int fd, char *filename, char *cgiargs)
{
    char buf[MAXLINE];
    char *emptylist[] = {NULL};
    
    //返回HTTP响应的开头部分
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    rio_writen(fd, buf, strlen(buf));

    if(fork() == 0)     //子进程
    {
        setenv("QUERY_STRING", cgiargs, 1);
        dup2(fd, STDOUT_FILENO);                //重定向输出流到客户端
        execve(filename, emptylist, environ);   //执行CGI程序
    }
    wait(NULL);         //父进程等待并回收子进程
}

void serve_dynamic_post(int fd, char *filename, int bodyLength, rio_t *rp)
{
    char buf[MAXLINE];
    char *emptylist[] = {NULL};
    char body[MAXLINE];
    int p[2];

    //返回HTTP响应的开头部分
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    rio_writen(fd, buf, strlen(buf));

    //读取http请求body部分进入管道
    pipe(p);
    memset(body, 0, MAXLINE);
    rio_readnb(rp, body, bodyLength);
    rio_writen(p[1], body, bodyLength);

    dup2(p[0], STDIN_FILENO);        //重定向标准输入流为p[0]

    //设置CONTENT-LENGTH环境变量
    char len[10];
    sprintf(len, "%d", bodyLength);
    setenv("CONTENT-LENGTH", (const char *)&len, 1);
    
    close(p[0]);
    close(p[1]);

    if(fork() == 0)     //子进程
    {
        dup2(fd, STDOUT_FILENO);                //重定向输出流到客户端
        execve(filename, emptylist, environ);   //执行CGI程序
    }
    wait(NULL);         //父进程等待并回收子进程

}

