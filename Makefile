CC =gcc
FLAG =-Wall -std=c99 -g
OBJS =httpd.o libhttp.o server.o connect.o request.o

.PHONY: all

all: httpd main

%.o:%.c
	$(CC) $(FLAG) -c $< -o $@

httpd: $(OBJS)
	$(CC) $(FLAG) $(OBJS) -o $@

main: main.c
	$(CC) $(FLAG) $< -o $@
