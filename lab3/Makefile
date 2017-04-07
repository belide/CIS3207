CC=gcc
CFLAGS=-g -Wall -pthread
RM=rm

all: clean server

server: main.c queue.c spellcheck.h
		$(CC) $(CFLAGS) -o server main.c queue.c

clean:
		$(RM) -rf server *.dSYM
