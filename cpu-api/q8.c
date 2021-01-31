#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUF_LEN 1024

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe failed\n");
        exit(1);
    }
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(2);
    } else if (rc == 0) {
        close(pipefd[0]);
        printf("child_1) I am going to write \"hello\" to the pipe\n");
        char *buf = "\"hello\"";
        if (write(pipefd[1], buf, strlen(buf)) >= 0) {
            printf("child_1) Done!\n");
        }
        close(pipefd[1]);
        return 0;
    } else {
        wait(NULL);
    }

    rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(2);
    } else if (rc == 0) {
        close(pipefd[1]);
        printf("child_2) I am going to receive what child_1 wrote from pipe\n");
        char buf[BUF_LEN];
        int size = read(pipefd[0], buf, BUF_LEN);
        if (size >= 0) {
            printf("child_2) What I received is %s\n", buf);
        }
        close(pipefd[0]);
        return 0;
    } else {
        wait(NULL);
    }
    printf("parent) goodbye\n");
    return 0;
}
