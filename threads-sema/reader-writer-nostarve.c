#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_threads.h"

//
// Your code goes in the structure and functions below
//

typedef struct __rwlock_t {
    sem_t lock, read, write;
    int read_cnt;
} rwlock_t;


void rwlock_init(rwlock_t *rw) {
    Sem_init(&rw->lock, 1);
    Sem_init(&rw->read, 1);
    Sem_init(&rw->write, 1);
    rw->read_cnt = 0;
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_t *lock = &rw->lock;
    sem_t *read = &rw->read;
    Sem_wait(read);
    Sem_wait(lock);
    int read_cnt = rw->read_cnt;
    read_cnt++;
    if (read_cnt == 1) {
        Sem_wait(&rw->write);
    }
    rw->read_cnt = read_cnt;
    Sem_post(lock);
    Sem_post(read);
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_t *lock = &rw->lock;
    Sem_wait(lock);
    int read_cnt = rw->read_cnt;
    read_cnt--;
    if (read_cnt == 0) {
        Sem_post(&rw->write);
    }
    rw->read_cnt = read_cnt;
    Sem_post(lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_t *read = &rw->read;
    sem_t *write = &rw->write;
    Sem_wait(read);
    Sem_wait(write);
    Sem_post(read);
}

void rwlock_release_writelock(rwlock_t *rw) {
    sem_t *write = &rw->write;
    Sem_post(write);
}

//
// Don't change the code below (just use it!)
// 

int loops;
int value = 0;

rwlock_t lock;

void *reader(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
	rwlock_acquire_readlock(&lock);
	printf("read %d\n", value);
	rwlock_release_readlock(&lock);
    }
    return NULL;
}

void *writer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
	rwlock_acquire_writelock(&lock);
	value++;
	printf("write %d\n", value);
	rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];

    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
	Pthread_create(&pr[i], NULL, reader, NULL);
    for (i = 0; i < num_writers; i++)
	Pthread_create(&pw[i], NULL, writer, NULL);

    for (i = 0; i < num_readers; i++)
	Pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
	Pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}

