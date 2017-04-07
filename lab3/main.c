#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "spellcheck.h"

int main(int argc, char **argv) {
    pthread_t threads[NUM_WORKERS];

    // listen socket descriptor
    // connected socket descriptor
    int listenfd, connectedfd;

    sockaddr client_addr;
    socklen_t client_addr_size;
    char line[MAX_LINE];
    ssize_t bytes_read;
    char client_name[MAX_LINE];
    char client_port[MAX_LINE];
    char *port;
    void *ret;

    if (argc < 2)
        port = DEFAULT_PORT_STR;
    else
        port = argv[1];

    listenfd = getlistenfd(port);

    // create worker threads
    for (int i; i < NUM_WORKERS; i++) {
        if(pthread_create(&threads[i], NULL, request_handle, NULL) != 0) {
            fprintf(stderr, "error creating thread.\n");
            return EXIT_FAILURE;
        }
    }

    while (1) {

    }

    // join threads
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (pthread_join(threads[i], &ret) != 0) {
            fprintf(stderr, "join error\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/* given a port number or service as string,
returns a descriptor to pass on to accept() */
int getlistenfd(char *port) {
    int listenfd, status;
    addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    // TCP
    hints.ai_socktype = SOCK_STREAM;
    // IPv4
    hints.ai_family = AF_INET;

    if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(EXIT_GETADDRINFO_ERROR);
    }

    /* try to bind to the first available address/port in the list
    if we fail, try the next one */
    for (p = res; p != NULL; p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }

    // free up memory
    freeaddrinfo(res);

    if (p == NULL)
        exit(EXIT_BIND_FAILURE);

    if (listen(listenfd, BACKLOG) < 0) {
        close(listenfd);
        exit(EXIT_LISTEN_FAILURE);
    }

    return listenfd;
}