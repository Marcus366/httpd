#ifndef __LIBHTTP_H__
#define __LIBHTTP_H__

int handle_new_connect(int listenfd, int epollfd);
int handle_read(int connfd, int epollfd);

#endif
