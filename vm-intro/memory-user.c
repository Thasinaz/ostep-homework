#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MEGA 1000000

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: memory-user <MB>\n");
        exit(1);
    }

    size_t size = atol(argv[1]) * MEGA;
    if (size == 0) {
        fprintf(stderr, "Usage: memory-user <MB>\n");
        exit(1);
    }
    long *memory = malloc(size);

    printf("pid: %d\n", getpid());

    size_t index = 0;
    while (1) {
        printf("%d %X\n", index, memory[index]);
        index++;
        if (index == size) {
            index = 0;
        }
        sleep(1);
    }
    return 0;
}

