CC =gcc
FLAG =-Wall -std=c99 -g
OBJS =httpd.o libhttp.o http_srv.o http_conn.o http_req.o

.PHONY: all

all: httpd run

%.o:%.c %.h
	$(CC) $(FLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(FLAG) $(OBJS) -o $@

run: httpd
	sudo ./httpd
