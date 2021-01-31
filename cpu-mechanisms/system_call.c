#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
    struct timeval t1, t2;
    char buf = 'c';
    for (int times = 0; times < 8; times++) {
        gettimeofday(&t1, NULL);
        for (int j = 0; j < 1024; j++) {
            read(STDIN_FILENO, &buf, 0);
        }
        gettimeofday(&t2, NULL);
        printf("%d\n", t2.tv_usec - t1.tv_usec);
    }
}
