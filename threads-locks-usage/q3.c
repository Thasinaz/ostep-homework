#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define MAXTHREAD 16
#define NUMCPUS 6
#define THRESHOLD 1024
#define IDMASK 0x1f

#define Pthread_mutex_init(mutex, attr) assert(pthread_mutex_init(mutex, attr) == 0)
#define Pthread_mutex_lock(mutex) assert(pthread_mutex_lock(mutex) == 0)
#define Pthread_mutex_unlock(mutex) assert(pthread_mutex_unlock(mutex) == 0)
#define Pthread_create(mutex, attr, routine, arg) assert(pthread_create(mutex, attr, routine, arg) == 0)
#define Pthread_join(mutex, retval) assert(pthread_join(mutex, retval) == 0)

typedef struct {
    long global;
    pthread_mutex_t glock;
    long local[NUMCPUS];
    pthread_mutex_t llock[NUMCPUS];
    long threshold;
} counter_t;

void init(counter_t *c, long threshold);
void increment(counter_t *c, long threadID);
long get(counter_t *c);


void *increment_n(void *p);


static counter_t counter;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <increment times> <number of threads>\n", argv[0]);
        exit(1);
    }

    pthread_t tid[MAXTHREAD];
    long inc_times = atol(argv[1]);
    long thread_num = atol(argv[2]);
    thread_num = MAXTHREAD < thread_num ? MAXTHREAD : thread_num;

    for (long i = 1; i <= thread_num; i++) {
        struct timeval beg_tv, end_tv;
        long times_per_thread = inc_times / i;
        init(&counter, THRESHOLD);

        assert(gettimeofday(&beg_tv, NULL) == 0);
        for (long j = 0; j < i; j++) {
            long args = (times_per_thread << 5) + j;
            Pthread_create(&tid[j], NULL, increment_n, (void *) args);
        }
        for (long j = 0; j < i; j++) {
            Pthread_join(tid[j], NULL);
        }
        assert(gettimeofday(&end_tv, NULL) == 0);

        long time = 1000000 * (end_tv.tv_sec - beg_tv.tv_sec) +
                    end_tv.tv_usec - beg_tv.tv_usec;

        printf("%ld per thread, %ld threads\nget %ld in %ld microsecond\n",
               times_per_thread, i, get(&counter), time);
    }
    return 0;
}

void *increment_n(void *p) {
    long times = ((unsigned long) p) >> 5;
    long id = ((long) p) & IDMASK;
    for (long i = 0; i < times; i++) {
        increment(&counter, id);
    }
    return NULL;
}


void init(counter_t *c, long threshold) {
    c->threshold = threshold;
    c->global = 0;
    Pthread_mutex_init(&c->glock, NULL);
    for (long i = 0; i < NUMCPUS; i++) {
        c->local[i] = 0;
        Pthread_mutex_init(&c->llock[i], NULL);
    }
}

void increment(counter_t *c, long threadID) {
    long cpu = threadID % NUMCPUS;
    Pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu]++;

    if (c->local[cpu] >= c->threshold) {
        Pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        Pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    Pthread_mutex_unlock(&c->llock[cpu]);
}

long get(counter_t *c) {
    Pthread_mutex_lock(&c->glock);
    long rc = c->global;
    Pthread_mutex_unlock(&c->glock);
    return rc;
}
