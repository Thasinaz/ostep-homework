#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

#define MAX_THREAD 1024

typedef struct __ns_mutex_t {
    sem_t lock, queue[MAX_THREAD];
    int head, tail;
} ns_mutex_t;

ns_mutex_t m;

void ns_mutex_init(ns_mutex_t *m) {
    Sem_init(&m->lock, 1);
    Sem_init(&m->queue[0], 1);
    for (int i = 1; i < MAX_THREAD; i++) {
        Sem_init(&m->queue[i], 0);
    }
    m->head = 0;
    m->tail = 0;
}

void ns_mutex_acquire(ns_mutex_t *m) {
    sem_t *lock = &m->lock;
    Sem_wait(lock);
    int tmp = m->tail;
    int tail = tmp;
    tail++;
    if (tail == MAX_THREAD) {
        tail = 0;
    }
    assert(tail != m->head);
    m->tail = tail;
    Sem_post(lock)
    Sem_wait(&m->queue[tmp]);
}

void ns_mutex_release(ns_mutex_t *m) {
    sem_t *lock = &m->lock;
    Sem_wait(lock);
    int head = m->head;
    head++;
    if (head == MAX_THREAD) {
        head = 0;
    }
    m->head = head;
    Sem_post(lock)
    Sem_post(&m->queue[head]);
}


void *worker(void *arg) {
    ns_mutex_acquire(&m);
    printf("child %ld\n", (long) arg);
    ns_mutex_release(&m);
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    long num_threads = atol(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];

    printf("parent: begin\n");
    ns_mutex_init(&m);

    long i;
    for (i = 0; i < num_threads; i++) {
        Pthread_create(&p[i], NULL, worker, (void *) i);
    }

    for (i = 0; i < num_threads; i++) {
        Pthread_join(p[i], NULL);
    }

    printf("parent: end\n");
    return 0;
}

