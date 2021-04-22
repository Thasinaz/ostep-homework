#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INIT 0

int main(int argc, char *argv[]) {
    FILE *in_fd, *out_fd;
    switch (argc) {
    case 1:
        in_fd = stdin;
        out_fd = stdout;
        break;
    case 2:
        if ((in_fd = fopen(argv[1], "r")) == NULL) {
            perror("fopen");
            return -1;
        }
        out_fd = stdout;
        break;
    case 3:
        if ((in_fd = fopen(argv[1], "r")) == NULL) {
            perror("fopen");
            return -1;
        }
        if ((out_fd = fopen(argv[2], "w")) == NULL) {
            perror("fopen");
            return -1;
        }
        break;
    default:
        fprintf(stderr, "Usage: %s [input_file] [output_file]\n", argv[0]);
        return -1;
    }

    uint8_t rc = INIT;
    uint8_t d;
    int counter = 0;
    while (fread(&d, 1, 1, in_fd) == 1) {
        rc ^= d;
        counter++;
        if (counter == 4000) {
            fwrite(&rc, 1, 1, out_fd);
            rc = INIT;
            counter = 0;
        }
    }
    if (ferror(in_fd)) {
        perror("fread");
        exit(-1);
    }
    if (counter != 0) {
        fwrite(&rc, 1, 1, out_fd);
    }

    fclose(in_fd);
    fclose(out_fd);
    return 0;
}
