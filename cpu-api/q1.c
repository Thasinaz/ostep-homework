#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int x = 100;
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("child) The value is %d\n", x);
        printf("child) and now I change it to 200\n");
        x = 200;
        printf("child) Now, the value is %d\n", x);
    } else {
        printf("parent) The value is %d\n", x);
        printf("parent) and now I change it to 300\n");
        x = 300;
        printf("parent) Now, the value is %d\n", x);
    }
    return 0;
}
