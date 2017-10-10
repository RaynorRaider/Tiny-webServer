# Tiny-webServer
a tiny web server

------

## 简介
可用的web服务器，仅支持http GET方法





## 主要功能
tiny可以打开指定端口接收http请求，给浏览器提供静态或动态内容，默认提供home.html






## 主要文件
> * RIO(Robust IO)包: 对系统级write以及read的封装，提供较为健壮的输入输出函数
> * mySocket包: 对系统级套接字的封装，提供API可以较为简单的打开客户端套接字，监听套接字
> * tiny.c及main.c: tiny webServer主要功能的实现及主函数
> * Makefile: 手写的Makefile



## 补充说明

* Makefile中存在将SockTest_cli.c 以及 SockTest_ser.c编译为可执行文件的语句，主要用于编写过程中测试mySocket提供的接口是否可用，对于tiny没有影响，如果重新编写mySocket包，则可以修改源文件继续用于测试。

* tiny服务器暂时
 >- 仅支持GET方法
 >- 支持单进程
 >- 对接收到的http报头仅读取不处理
 >- 缺少对动态内容执行失败的处理

* 动态内容默认目录为cgi-bin，可以自行创建该目录并放入测试用例
