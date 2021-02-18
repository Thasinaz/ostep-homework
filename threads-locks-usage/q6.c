#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define MAXTHREAD 16
#define NUMCPUS 6
#define MINDEGREE 164

#define Pthread_mutex_init(mutex, attr) assert(pthread_mutex_init(mutex, attr) == 0)
#define Pthread_mutex_lock(mutex) assert(pthread_mutex_lock(mutex) == 0)
#define Pthread_mutex_unlock(mutex) assert(pthread_mutex_unlock(mutex) == 0)
#define Pthread_create(mutex, attr, routine, arg) assert(pthread_create(mutex, attr, routine, arg) == 0)
#define Pthread_join(mutex, retval) assert(pthread_join(mutex, retval) == 0)

void *Malloc(size_t size) {
    void *new = malloc(size);

    if (new == NULL) {
        fprintf(stderr, "malloc error\n");
        _exit(1);
    }
    return new;
}

typedef struct __node_t {
    int key[2 * MINDEGREE - 1];
    struct __node_t *c[2 * MINDEGREE];
    int leaf;
    int n;
    pthread_mutex_t lock;
} node_t;

typedef struct __B_tree {
    node_t *root;
    pthread_mutex_t lock;
} B_tree;

void B_tree_Init(B_tree *T);
void B_tree_Insert(B_tree *T, int key);
int B_tree_Lookup(B_tree *T, int key);


typedef struct {
    long times;
    int type;
    int range_beg;
    int range_end;
} task_t;

void *work(void *p);


static B_tree T;
static task_t task_list[MAXTHREAD];

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <number of operation> <ratio of insert to lookup> <number of threads> <ratio...>\n", argv[0]);
        exit(1);
    }

    B_tree_Init(&T);

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
            B_tree_Insert(&T, seqnum);
        } else {
            B_tree_Lookup(&T, seqnum);
        }
    }
    return NULL;
}


void B_tree_Init(B_tree *T) {
    node_t *new = Malloc(sizeof(node_t));
    new->leaf = 1;
    new->n = 0;
    Pthread_mutex_init(&new->lock, NULL);
    T->root = new;
    Pthread_mutex_init(&T->lock, NULL);
}

void B_tree_split_child(node_t *node, int pos) {
    node_t *new = Malloc(sizeof(node_t));
    node_t *child = node->c[pos];
    pthread_mutex_t *child_lock = &child->lock;
    Pthread_mutex_lock(child_lock);
    int leaf = child->leaf;
    for (int i = 0; i < MINDEGREE - 1; i++) {
        new->key[i] = child->key[i + MINDEGREE];
    }
    if (!leaf) {
        for (int i = 0; i < MINDEGREE; i++) {
            new->c[i] = child->c[i + MINDEGREE];
        }
    }
    child->n = MINDEGREE - 1;
    int tmp = child->key[MINDEGREE - 1];
    Pthread_mutex_unlock(child_lock);
    for (int i = node->n; i > pos; i--) {
        node->c[i + 1] = node->c[i];
    }
    node->c[pos + 1] = new;
    for (int i = node->n; i > pos; i--) {
        node->key[i] = node->key[i - 1];
    }
    node->key[pos] = tmp;
    new->leaf = leaf;
    new->n = MINDEGREE - 1;
    Pthread_mutex_init(&new->lock, NULL);
    node->n++;
}

void B_tree_Insert_nonfull(node_t *node, int key) {
    int i = node->n - 1;
    if (node->leaf) {
        while (i >= 0 && node->key[i] > key) {
            node->key[i + 1] = node->key[i];
            i--;
        }
        node->key[i + 1] = key;
        node->n++;
        Pthread_mutex_unlock(&node->lock);
    } else {
        while (i >= 0 && node->key[i] > key) {
            i--;
        }
        i++;
        if (node->c[i]->n == 2 * MINDEGREE - 1) {
            B_tree_split_child(node, i);
            if (key > node->key[i]) {
                i++;
            }
        }
        Pthread_mutex_lock(&node->c[i]->lock);
        Pthread_mutex_unlock(&node->lock);
        B_tree_Insert_nonfull(node->c[i], key);
    }
}

void B_tree_Insert(B_tree *T, int key) {
    pthread_mutex_t *lock = &T->lock;
    Pthread_mutex_lock(lock);
    node_t *root = T->root;
    if (root->n == 2 * MINDEGREE - 1) {
        node_t *new = Malloc(sizeof(node_t));
        T->root = new;
        pthread_mutex_t *root_lock = &new->lock;
        Pthread_mutex_init(root_lock, NULL);
        Pthread_mutex_lock(root_lock);
        Pthread_mutex_unlock(lock);
        new->leaf = 0;
        new->n = 0;
        new->c[0] = root;
        B_tree_split_child(new, 0);
        B_tree_Insert_nonfull(new, key);
    } else {
        Pthread_mutex_lock(&root->lock);
        Pthread_mutex_unlock(lock);
        B_tree_Insert_nonfull(root, key);
    }
}

int B_tree_Lookup(B_tree *T, int key) {
    return 0;
    pthread_mutex_t *lock = &T->lock;
    int rv = -1;
    Pthread_mutex_lock(lock);
    node_t *node = T->root;
    Pthread_mutex_unlock(lock);
    while (1) {
        lock = &node->lock;
        Pthread_mutex_lock(lock);
        int i = 0, n = node->n;
        while (i < n && node->key[i] < key) {
            i++;
        }
        if (i < n && node->key[i] == key) {
            rv = 0;
            break;
        } else if (node->leaf){
            break;
        } else {
            node = node->c[i];
            Pthread_mutex_unlock(lock);
        }
    }
    Pthread_mutex_unlock(lock);
    return rv;
}
