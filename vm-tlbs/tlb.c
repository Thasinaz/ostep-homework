#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>

#define NS_PER_S ((long) 1e9)
#define NS_PER_US ((long) 1e3)

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <NUMPAGES> <NUMTRIALS>\n", *argv);
        exit(1);
    }

    long num_pages = atol(argv[1]), num_trials = atol(argv[2]),
        jump = getpagesize() / sizeof(int);
    long size = num_pages * jump;
    struct timeval beg_tv;
    struct timeval end_tv;
    cpu_set_t set;
    int *a = malloc(size * sizeof(int));
    if(a == NULL) {
        exit(1);
    }

    CPU_ZERO(&set);
    CPU_SET(0, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        exit(1);
    }

    for (long j = 0; j < size; j += jump) {
        a[j] += 1;
    }

    if (gettimeofday(&beg_tv, NULL) != 0) {
        exit(1);
    }

    for (long i = 0; i < num_trials; i++) {
        for (long j = 0; j < size; j += jump) {
            a[j] += 1;
        }
    }

    if (gettimeofday(&end_tv, NULL) != 0) {
        exit(1);
    }

    time_t sec = end_tv.tv_sec - beg_tv.tv_sec;
    suseconds_t usec = end_tv.tv_usec - beg_tv.tv_usec;

    printf("%ld\n", (sec * NS_PER_S + usec * NS_PER_US) / (num_pages * num_trials));

    free(a);
    return 0;
}
