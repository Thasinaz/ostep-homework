#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define MAXTHREAD 16
#define NUMCPUS 6

#define Pthread_mutex_init(mutex, attr) assert(pthread_mutex_init(mutex, attr) == 0)
#define Pthread_mutex_lock(mutex) assert(pthread_mutex_lock(mutex) == 0)
#define Pthread_mutex_unlock(mutex) assert(pthread_mutex_unlock(mutex) == 0)
#define Pthread_create(mutex, attr, routine, arg) assert(pthread_create(mutex, attr, routine, arg) == 0)
#define Pthread_join(mutex, retval) assert(pthread_join(mutex, retval) == 0)

typedef struct __node_t {
    int key;
    pthread_mutex_t lock;
    struct __node_t *next;
} node_t;

typedef struct __list_t {
    node_t *head;
    pthread_mutex_t lock;
} list_t;

void List_Init(list_t *L);
void List_Insert(list_t *L, int key);
int List_Lookup(list_t *L, int key);


typedef struct {
    long times;
    int type;
    int range_beg;
    int range_end;
} task_t;

void *work(void *p);


static list_t L;
static task_t task_list[MAXTHREAD];

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <number of operation> <ratio of insert to lookup> <number of threads> <ratio...>\n", argv[0]);
        exit(1);
    }

    List_Init(&L);

    pthread_t tid[MAXTHREAD];
    long operation_times = atol(argv[1]);
    long operation_ratio = atol(argv[2]);
    long thread_num = atol(argv[3]);
    long thread_ratio = atol(argv[4]);
    thread_num = MAXTHREAD < thread_num ? MAXTHREAD : thread_num;

    long insert_times = operation_times * operation_ratio / 100;
    long lookup_times = operation_times * (100 -  operation_ratio) / 100;
    long insert_threads = thread_num * thread_ratio / 100;
    long lookup_threads = thread_num * (100 - thread_ratio) / 100;

    long insert_per_thread = insert_times / insert_threads;
    long lookup_per_thread = lookup_times / lookup_threads;

    task_t *insert_task = task_list;
    task_t *lookup_task = task_list + insert_threads;

    for (long i = 0; i < insert_threads; i++) {
        insert_task[i].times = insert_per_thread;
        insert_task[i].type = 1;
        insert_task[i].range_beg = i * insert_per_thread % (long) 1e6;
        insert_task[i].range_end = i * insert_per_thread % (long) 1e6 + insert_per_thread;
    }

    for (long i = 0; i < lookup_threads; i++) {
        lookup_task[i].times = lookup_per_thread;
        lookup_task[i].type = 0;
        lookup_task[i].range_beg = i * lookup_per_thread % (long) 1e6;
        lookup_task[i].range_end = i * lookup_per_thread % (long) 1e6 + lookup_per_thread;
    }

    struct timeval beg_tv, end_tv;
    assert(gettimeofday(&beg_tv, NULL) == 0);
    for (long i = 0; i < insert_threads; i++) {
        task_t* args = insert_task + i;
        Pthread_create(&tid[i], NULL, work, (void *) args);
    }
    for (long i = 0; i < lookup_threads; i++) {
        task_t* args = lookup_task + i;
        Pthread_create(&tid[insert_threads + i], NULL, work, (void *) args);
    }

    for (long i = 0; i < thread_num; i++) {
        Pthread_join(tid[i], NULL);
    }
    assert(gettimeofday(&end_tv, NULL) == 0);

    long time = 1000000 * (end_tv.tv_sec - beg_tv.tv_sec) +
                end_tv.tv_usec - beg_tv.tv_usec;

    printf("%ld\n", time);
    return 0;
}

void *work(void *p) {
    task_t *t = (task_t *) p;
    long type = t->type;
    long times = t->times;
    long range_beg = t->range_beg;
    long range_end = t->range_end;
    long seqnum = t->range_beg;
    for (long seqnum = t->range_beg; times--; seqnum++) {
        if (seqnum == range_end) {
            seqnum = range_beg;
        }
        if (type) {
            List_Insert(&L, seqnum);
        } else {
            List_Lookup(&L, seqnum);
        }
    }
    return NULL;
}


void List_Init(list_t *L) {
    L->head = NULL;
    Pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return;
    }
    new->key = key;
    Pthread_mutex_init(&new->lock, NULL);

    pthread_mutex_t *head_lock = &L->lock;
    Pthread_mutex_lock(head_lock);
    new->next = L->head;
    L->head = new;
    Pthread_mutex_unlock(head_lock);
}

int List_Lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_t *head_lock = &L->lock;
    Pthread_mutex_lock(head_lock);
    pthread_mutex_t *pre_lock = head_lock;
    for (node_t *curr = L->head; curr; curr = curr->next) {
        pthread_mutex_t *curr_lock = &curr->lock;
        Pthread_mutex_lock(curr_lock);
        Pthread_mutex_unlock(pre_lock);
        if (curr->key == key) {
            rv = 0;
            pre_lock = curr_lock;
            break;
        }
        pre_lock = curr_lock;
    }
    Pthread_mutex_unlock(pre_lock);
    return rv;
}
