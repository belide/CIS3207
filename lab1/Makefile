CC=gcc
CFLAGS=-g -Wall -std=c99
RM=rm

all: clean application.c timer1.c timer2.c
	$(CC) $(CFLAGS) -o application application.c
	$(CC) $(CFLAGS) -o timer1 timer1.c
	$(CC) $(CFLAGS) -o timer2 timer2.c

clean:
	$(RM) -rf application timer1 timer2 *.dSYM