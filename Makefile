CC =gcc
FLAG =-Wall -std=c99 -g
OBJS =httpd.o libhttp.o http_srv.o http_conn.o http_req.o http_res.o http_timer.o

.PHONY: all

all: httpd run clean

%.o:%.c %.h
	$(CC) $(FLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(FLAG) $(OBJS) -o $@

run: httpd
	sudo ./httpd

clean:
	rm *.o
	rm httpd
