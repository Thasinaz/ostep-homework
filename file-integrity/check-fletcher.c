#include <stdio.h>

#define INIT_S1 0
#define INIT_S2 0

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

    unsigned char s1 = INIT_S1,
                  s2 = INIT_S2;

    unsigned char d;
    while (fread(&d, 1, 1, fd) == 1) {
        s1 = (s1 + d) % 255;
        s2 = (s2 + s1) % 255;
    }
    if (ferror(fd)) {
        perror("fread");
        return -1;
    }

    unsigned short rc = ((short) s1 << 8) + (short) s2;
    fwrite(&rc, 2, 1, stdout);
    fclose(fd);
    return 0;
}
