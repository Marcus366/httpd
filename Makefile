CC=gcc
CFLAG=-Wall -std=gnu99 -g
OBJS=httpd.o								\
			http_listen_socket.o	\
			http_server.o					\
			http_connection.o			\
			http_request.o				\
			http_res.o						\
			http_timer.o					\
			http_log.o						\
			http_config.o					\
			http_fcache.o					\
			http_header.o					\
			http_event.o					\
			http_mem.o

INSTALL=install
INSTALL_FLAGS=-c -m 755

.PHONY: all


%.o:%.c %.h
	$(CC) $(CFLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -llua -lm -o $@

clean:
	rm *.o
	rm httpd

install: httpd
	$(INSTALL) $(INSTALL_FLAGS) httpd /usr/local/bin/
	test -d /etc/httpd || mkdir /etc/httpd
	cp config/httpd.conf /etc/httpd/httpd.conf
	touch /etc/httpd/httpd.pid && chmod 644 /etc/httpd/httpd.pid

all: httpd run clean

