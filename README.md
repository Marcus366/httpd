httpd
=====
my toy linux http server

What is this project?
------------------------
This is a simple web server borned just as a toy. It is of no use except help my understanding of web server.

It use single process/thread epoll IO module, and use linux sendfile as main transfer function, which make it tackle static file as fast as nginx.


Build Dependence
-----------------------
**lua5.2
