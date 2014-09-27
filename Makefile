CC =gcc
FLAG =-Wall -std=c99 -g
OBJS =httpd.o http_srv.o http_conn.o http_req.o http_res.o http_timer.o http_log.o http_conf.o

.PHONY: all

run: httpd
	sudo ./httpd

%.o:%.c %.h
	$(CC) $(FLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(FLAG) $(OBJS) -o $@

clean:
	rm *.o
	rm httpd

all: httpd run clean
