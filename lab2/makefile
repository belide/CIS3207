# Tu Tran - Operating Systems Project 2
# Created at: 7:02 AM March 2nd, 2017

CC=gcc
CFLAGS=-g -Wall -std=c99

all: clean shell

shell: shell.c util.c shell.h
	$(CC) $(CFLAGS) -o shell shell.c util.c

clean:
	$(RM) -rf shell *.dSYM