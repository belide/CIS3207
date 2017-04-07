#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "spellcheck.h"

void q_init(queue *q, int n) {
    q->buf = calloc(n, sizeof(int));
    // capacity is n
    q->capacity = n;
    // empty buffer if front == rear
    q->front = q->rear = 0;
    // binary sem for locking
    sem_init(&q->mutex, 0, 1);
    // initially buf has n empty slots
    sem_init(&q->slots, 0, n);
    // initially buf has 0 items
    sem_init(&q->items, 0, 0);
}

void q_deinit(queue *q) {
    free(q->buf);
}

void q_insert(queue *q, int item) {
    P(&q->slots);
    P(&q->mutex);
    q->buf[(++q->rear) % (q->capacity)] = item;
    V(&q->mutex);
    V(&q->items);
}

void q_remove(queue *q) {
    int item;
    P(&q->items);
    P(&q->mutex);
    item = q->buf[(++q->front) % (q->capacity)];
    V(&q->mutex);
    V(&q->slots);
    return item;
}