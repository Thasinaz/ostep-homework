#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define POLY 0x1021
#define INIT 0xffff

uint16_t crc(FILE *fd) {
    uint16_t rc;
    rc = INIT;
    uint8_t d;
    while (fread(&d, 1, 1, fd) == 1) {
        for (int i = 7; i >= 0; i--) {
            uint16_t bit = (d >> i) & 1;
            if (rc & 0x8000) {
                rc = (rc << 1) + bit;
                rc ^= POLY;
            } else {
                rc = (rc << 1) + bit;
            }
        }
    }
    if (ferror(fd)) {
        perror("fread");
        exit(-1);
    }
    for (int i = 0; i < 16; i++) {
        if (rc & 0x8000) {
            rc = (rc << 1);
            rc ^= POLY;
        } else {
            rc = (rc << 1);
        }
    }
    return rc;
}

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

    uint16_t rc = crc(fd);
    fwrite(&rc, 2, 1, stdout);
    fclose(fd);
    return 0;
}
