CC =gcc
CFLAG =-Wall -std=gnu99 -g
OBJS =httpd.o					\
			http_srv.o			\
			http_conn.o 		\
			http_request.o	\
			http_res.o			\
			http_timer.o		\
			http_log.o			\
			http_config.o		\
			http_fcache.o		\
			http_header.o		\
			string_utils.o

.PHONY: all httpd_debug

run: httpd
	sudo ./httpd

debug: $(OBJS)
	sudo sh debug.sh

%.o:%.c %.h
	$(CC) $(CFLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -llua -lm -o $@

clean:
	rm *.o
	rm httpd

all: httpd run clean
