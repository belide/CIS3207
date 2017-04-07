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
    // size is 0
    q->size = 0;
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
    sem_wait(&q->slots);
    sem_wait(&q->mutex);
    q->buf[(++q->rear) % (q->capacity)] = item;
    q->size++;
    sem_post(&q->mutex);
    sem_post(&q->items);
}

int q_remove(queue *q) {
    int item;
    sem_wait(&q->items);
    sem_wait(&q->mutex);
    item = q->buf[(++q->front) % (q->capacity)];
    q->size--;
    sem_post(&q->mutex);
    sem_post(&q->slots);
    return item;
}