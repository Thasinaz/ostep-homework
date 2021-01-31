#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t ppid = getpid();
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        pid_t rc_wait = wait(NULL);
        printf("child) The value return from wait is %d\nand the pid of parent is %d\n", rc_wait, ppid);
    } else {
        pid_t rc_wait = wait(NULL);
        printf("parent) The value return from wait is %d\nand the pid of the child is %d\n", rc_wait, rc);
    }
    return 0;
}
