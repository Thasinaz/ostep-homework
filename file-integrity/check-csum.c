#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INIT 0

int main(int argc, char *argv[]) {
    FILE *file_fd, *csum_fd;
    switch (argc) {
    case 2:
        if ((file_fd = fopen(argv[1], "r")) == NULL) {
            perror("fopen");
            return -1;
        }
        csum_fd = stdin;
        break;
    case 3:
        if ((file_fd = fopen(argv[1], "r")) == NULL) {
            perror("fopen");
            return -1;
        }
        if ((csum_fd = fopen(argv[2], "r")) == NULL) {
            perror("fopen");
            return -1;
        }
        break;
    default:
        fprintf(stderr, "Usage: %s <file> [check_sum]\n", argv[0]);
        return -1;
    }

    uint8_t rc = INIT;
    uint8_t d;
    int counter = 0;
    while (fread(&d, 1, 1, file_fd) == 1) {
        rc ^= d;
        counter++;
        if (counter == 4000) {
            uint8_t csum;
            int tmp = fread(&csum, 1, 1, csum_fd);
            if (tmp == 0) {
                if (ferror(file_fd)) {
                    perror("fread");
                    exit(-1);
                } else {
                    printf("file corrupted\n");
                    return 0;
                }
            }
            if (rc != csum) {
                printf("file corrupted\n");
                return 0;
            }
            rc = INIT;
            counter = 0;
        }
    }
    if (ferror(file_fd)) {
        perror("fread");
        exit(-1);
    }
    if (counter != 0) {
        uint8_t csum;
        int tmp = fread(&csum, 1, 1, csum_fd);
        if (tmp == 0) {
            if (ferror(file_fd)) {
                perror("fread");
                exit(-1);
            } else {
                printf("file corrupted\n");
                return 0;
            }
        }
        if (rc != csum) {
            printf("file corrupted\n");
            return 0;
        }
    }

    fclose(file_fd);
    fclose(csum_fd);
    return 0;
}
