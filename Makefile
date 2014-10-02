CC =gcc
CFLAG =-Wall -std=gnu99 -O2
OBJS =httpd.o http_srv.o http_conn.o http_req.o http_res.o http_timer.o http_log.o http_conf.o \
	  http_fcache.o

.PHONY: all httpd_debug

run: httpd
	sudo ./httpd

debug: $(OBJS)
	sudo sh debug.sh

%.o:%.c %.h
	$(CC) $(CFLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -o $@

clean:
	rm *.o
	rm httpd

all: httpd run clean
