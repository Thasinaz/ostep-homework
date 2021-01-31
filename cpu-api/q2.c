#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_LEN 1024

int main() {
    int fd = open("./q2.dat", O_CREAT|O_RDWR, S_IRWXU);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("child) The content of the file is\n");
        char buf[BUF_LEN];
        int size = read(fd, buf, BUF_LEN);
        buf[size] = 0;
        printf("%s", buf);
        printf("child) Now, I write \"child) hello\" to the file\n");
        char *content = "\"child) hello\"\n";
        write(fd, content, strlen(content));
        printf("child) Now, content of the file is\n");
        size = read(fd, buf, BUF_LEN);
        buf[size] = 0;
        printf("%s", buf);
        close(fd);
    } else {
        printf("parent) The content of the file is\n");
        char buf[BUF_LEN];
        int size = read(fd, buf, BUF_LEN);
        buf[size] = 0;
        printf("%s", buf);
        printf("parent) Now, I write \"parent) hello\" to the file\n");
        char *content = "\"parent) hello\"\n";
        write(fd, content, strlen(content));
        printf("parent) Now, content of the file is\n");
        size = read(fd, buf, BUF_LEN);
        buf[size] = 0;
        printf("%s", buf);
        close(fd);
    }
    return 0;
}
