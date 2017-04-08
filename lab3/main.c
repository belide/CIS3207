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
    int i;

    sockaddr client_addr;
    socklen_t client_addr_size;
    char client_name[MAX_LINE];
    char client_port[MAX_LINE];
    char *port;
    void *ret;
    queue q;

    if (argc < 2)
        port = DEFAULT_PORT_STR;
    else
        port = argv[1];

    listenfd = getlistenfd(port);

    // initializing queue
    q_init(&q, 20);

    // create worker threads
    for (i = 0; i < NUM_WORKERS; i++) {
        if(pthread_create(&threads[i], NULL, request_handle, &q) != 0) {
            fprintf(stderr, "error creating thread.\n");
            return EXIT_FAILURE;
        }
        // printf("DEBUG: Creating thread %d\n", threads[i]);
    }

    while (1) {
        // printf("DEBUG: inside main while loop\n");
        client_addr_size = sizeof(client_addr);

        if((connectedfd = accept(listenfd, (sockaddr *) &client_addr, &client_addr_size)) == -1) {
            fprintf(stderr, "accept error\n");
            continue;
        }

        // printf("DEBUG: after connect()\n");

        if (getnameinfo((sockaddr *) &client_addr, client_addr_size, client_name, MAX_LINE, client_port, MAX_LINE, 0) != 0) {
            fprintf(stderr, "error getting name information about client\n");
        } else {
            printf("accepted connection from %s: %s\nWaiting for handling threads\n", client_name, client_port);
        }

        printf("DEBUG: connectedfd is %d\n", connectedfd);

        q_insert(&q, connectedfd);

        printf("DEBUG: number of connections is %d\n", q.size);
    }

    // join threads
    for (i = 0; i < NUM_WORKERS; i++) {
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
    for (p = res; p != NULL; p = p->ai_next) {
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

    printf("SUCCESSFULLY created socket descriptor\n");
    printf("Listening on port %s\n", port);

    return listenfd;
}

void *request_handle(void *q) {
    int connectedfd;
    ssize_t bytes_read;
    char line[MAX_LINE];

    queue *qu = (queue *) q;

    while (1) {
        printf("DEBUG: (inside thread) number of connections is %d\n", qu->size);
        connectedfd = q_remove(qu);

        printf("DEBUG: just removed socket des %d from queue\n", connectedfd);

        while((bytes_read = readLine(connectedfd, line, MAX_LINE - 1)) > 0) {
            printf("just read %s", line);
            write(connectedfd, line, bytes_read);
        }

        printf("connection closed\n");
        close(connectedfd);
    }
}

/* FROM KERRISK 

   Read characters from 'fd' until a newline is encountered. If a newline
   character is not encountered in the first (n - 1) bytes, then the excess
   characters are discarded. The returned string placed in 'buf' is
   null-terminated and includes the newline character if it was read in the
   first (n - 1) bytes. The function return value is the number of bytes
   placed in buffer (which includes the newline character if encountered,
   but excludes the terminating null byte). */
ssize_t readLine(int fd, void *buffer, size_t n) {
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
        if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
            totRead++;
            *buf++ = ch;
        }

        if (ch == '\n')
            break;
        }
    }

    *buf = '\0';
    return totRead;
}
