CC=gcc
CFLAGS=-g -Wall -std=c99
RM=rm

all: clean server

server: main.c queue.c spellcheck.h
		$(CC) $(CFLAGS) -o server main.c queue.c

clean:
		$(RM) -rf server *.dSYM