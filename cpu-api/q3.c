#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void print_good_bye() {
    printf("parent) goodbye\n");
}

int main() {
    int rc = fork();
    pid_t ppid = getpid();
    signal(SIGINT, print_good_bye);
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("child) hello\n");
        kill(ppid, SIGINT);
    }
    return 0;
}
