#include <stdio.h>

#define INIT 0

int main(int argc, char *argv[]) {
    FILE *fd;
    switch (argc) {
    case 1:
        fd = stdin;
        break;
    case 2:
        if ((fd = fopen(argv[1], "r")) == NULL) {
            perror("fopen");
            return -1;
        }
        break;
    default:
        fprintf(stderr, "Usage: %s [pathname]\n", argv[0]);
        return -1;
    }

    unsigned char rc = INIT;
    unsigned char d;
    while (fread(&d, 1, 1, fd) == 1) {
        rc ^= d;
    }
    if (ferror(fd)) {
        perror("fread");
        return -1;
    }

    fwrite(&rc, 1, 1, stdout);
    fclose(fd);
    return 0;
}
