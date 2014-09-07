CC = gcc
FLAG = -Wall -g

.PHONY: all

all: ehttpd main

ehttpd: ehttpd.c
	$(CC) $(FLAG) $< -o $@	

main: main.c
	$(CC) $(FLAG) $< -o $@

