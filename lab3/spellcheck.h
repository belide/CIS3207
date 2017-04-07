#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <semaphore.h>

#define BUF_SIZE 256
#define _XOPEN_SOURCE 600

#define READ 0
#define WRITE 1

#define TRUE 1
#define FALSE 0

#define BACKLOG 20
#define DEFAULT_PORT_STR "8000"

#define DEFAULT_DICTIONARY "words"

#define EXIT_USAGE_ERROR 1
#define EXIT_GETADDRINFO_ERROR 2
#define EXIT_BIND_FAILURE 3
#define EXIT_LISTEN_FAILURE 4

#define NUM_WORKERS 10

#define MAX_LINE 64

typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct sockaddr sockaddr;
typedef struct {
    // buffer array
    int *buf;
    // maximum number of slots
    int capacity;
    // first item
    int front;
    // last item
    int rear;
    // number of items
    int size;
    sem_t mutex;
    sem_t slots;
    sem_t items;
} queue;

// queue thing
void q_init(queue *, int);
void q_deinit(queue *);
void q_insert(queue *, int);
int q_remove(queue *);

int getlistenfd(char *);
void *request_handle(void *);
ssize_t readLine(int , void *, size_t);

#endif
