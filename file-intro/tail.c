#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const size_t exp_line_len = 80;

static int counter = 0;
static size_t file_size = 0;

void print_usage(char *s) {
    fprintf(stderr, "Usage: %s -<n> file", s);
}

void print_lines(int fd, size_t begin, size_t size) {
    char buf[size];
    read(fd, buf, size);
    char *pos = buf + size - 1;
    while (pos >= buf) {
        if ((*pos) == '\n') {
            counter--;
        }
        if (counter == 0) {
            write(STDOUT_FILENO, pos + 1, buf + size - pos - 1);
            return;
        }
    }
    if (begin == 0) {
        write(STDOUT_FILENO, buf, size);
        return;
    }
    size_t new_begin = begin < counter * exp_line_len ? 0 : begin - counter * exp_line_len;
    lseek(fd, new_begin, SEEK_SET);
    print_lines(fd, new_begin, begin - new_begin);
    write(STDOUT_FILENO, buf, size);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || argv[1][0] != '-' || argv[1][1] == 0) {
        print_usage(argv[0]);
        exit(1);
    }

    counter = atoi(argv[1] + 1);

    int fd;
    if ((fd = open(argv[0], O_RDONLY)) == -1) {
        perror("tail");
        exit(1);
    }
    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        perror("tail");
        exit(1);
    }
    file_size = statbuf.st_size;
    size_t begin = file_size < counter * exp_line_len ? 0 : file_size - counter * exp_line_len;
    lseek(fd, begin, SEEK_SET);
    print_lines(fd, begin, file_size - begin);
    close(fd);
    return 0;
}
