CC=gcc
CFLAG=-Wall -std=gnu99 -g
SRC_PATH=src
OBJS_PATH=objs
OBJS=$(OBJS_PATH)/httpd.o								\
			$(OBJS_PATH)/http_listen_socket.o	\
			$(OBJS_PATH)/http_server.o				\
			$(OBJS_PATH)/http_connection.o		\
			$(OBJS_PATH)/http_request.o				\
			$(OBJS_PATH)/http_timer.o					\
			$(OBJS_PATH)/http_log.o						\
			$(OBJS_PATH)/http_config.o				\
			$(OBJS_PATH)/http_fcache.o				\
			$(OBJS_PATH)/http_header.o				\
			$(OBJS_PATH)/http_event.o					\
			$(OBJS_PATH)/http_mempool.o				\
			$(OBJS_PATH)/http_chain.o					\
			$(OBJS_PATH)/http_mem.o

INSTALL=install
INSTALL_FLAGS=-c -m 755

.PHONY: all httpd

httpd:
	test -d objs || mkdir objs
	make $(OBJS_PATH)/httpd

$(OBJS_PATH)/%.o:$(SRC_PATH)/%.c $(SRC_PATH)/%.h $(SRC_PATH)/http_log.h
	$(CC) $(CFLAG) -c $< -o $@

$(OBJS_PATH)/httpd: $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -llua -lm -o $@

clean:
	rm -rf objs/*.o
	rm objs/httpd

install: $(OBJS_PATH)/httpd
	$(INSTALL) $(INSTALL_FLAGS) $(OBJS_PATH)/httpd /usr/local/bin/
	test -d /usr/local/www || mkdir /usr/local/www
	test -d /etc/httpd || mkdir /etc/httpd
	cp config/index.html /usr/local/www/index.html
	cp config/httpd.conf /etc/httpd/httpd.conf
	touch /etc/httpd/httpd.pid && chmod 644 /etc/httpd/httpd.pid

all: $(OBJS_PATH)/httpd clean

