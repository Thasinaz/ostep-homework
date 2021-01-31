#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
    int pipefd_1[2], pipefd_2[2];
    cpu_set_t set;
    struct timeval t1, t2;
    int nloops = 1024;

    if (pipe(pipefd_1) == -1) {
        fprintf(stderr, "pipe failed\n");
        exit(1);
    } else if (pipe(pipefd_2) == -1) {
        fprintf(stderr, "pipe failed\n");
        exit(1);
    }

    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof(cpu_set_t), &set);

    gettimeofday(&t1, NULL);

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(2);
    } else if (rc == 0) {
        char s[6] = "Hello";
        for (int i = 0; i < nloops; i++) {
            read(pipefd_1[0], s, 5);
            //printf("child) you said \"%s\"\n", s);
            write(pipefd_2[1], s, 5);
            //printf("child) %s\n", s);
        }
    } else {
        char s[6] = "Hello";
        write(pipefd_1[1], s, 5);
        //printf("parent) %s\n", s);
        for (int i = 0; i < nloops; i++) {
            read(pipefd_2[0], s, 5);
            //printf("parent) you said \"%s\"\n", s);
            write(pipefd_1[1], s, 5);
            //printf("parent) %s\n", s);
        }
    }
    gettimeofday(&t2, NULL);
    printf("%d\n", t2.tv_usec - t1.tv_usec);
}
