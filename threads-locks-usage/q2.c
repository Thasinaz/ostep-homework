#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define MAXTHREAD 16

#define Pthread_mutex_init(mutex, attr) assert(pthread_mutex_init(mutex, attr) == 0)
#define Pthread_mutex_lock(mutex) assert(pthread_mutex_lock(mutex) == 0)
#define Pthread_mutex_unlock(mutex) assert(pthread_mutex_unlock(mutex) == 0)
#define Pthread_create(mutex, attr, routine, arg) assert(pthread_create(mutex, attr, routine, arg) == 0)
#define Pthread_join(mutex, retval) assert(pthread_join(mutex, retval) == 0)

typedef struct {
    long value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c);
void increment(counter_t *c);
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
        init(&counter);

        assert(gettimeofday(&beg_tv, NULL) == 0);
        for (long j = 0; j < i; j++) {
            Pthread_create(&tid[j], NULL, increment_n, (void *) times_per_thread);
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
    long times = (long) p;

    for (long i = 0; i < times; i++) {
        increment(&counter);
    }
    return NULL;
}


void init(counter_t *c) {
    c->value = 0;
    Pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    c->value++;
    Pthread_mutex_unlock(&c->lock);
}

long get(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    long rc = c->value;
    Pthread_mutex_unlock(&c->lock);
    return rc;
}
