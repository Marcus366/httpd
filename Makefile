CC =gcc
CFLAG =-Wall -std=gnu99 -O2
OBJS =httpd.o								\
			http_srv.o						\
			http_connection.o			\
			http_request.o				\
			http_res.o						\
			http_timer.o					\
			http_log.o						\
			http_config.o					\
			http_fcache.o					\
			http_header.o					\
			http_mem.o

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

