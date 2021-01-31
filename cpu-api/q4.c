#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        char *args[4];
        execl("/bin/ls", "ls", (char *)NULL);
    } else {
        wait(NULL);
    }
    return 0;
}
