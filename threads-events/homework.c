#include <aio.h>

#include "csapp.h"

void process_request(int cd) {
    Close(cd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int sd = Open_listenfd(argv[1]);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(sd, &set);

    while (1) {
        fd_set tmp = set;
        Select(sd + 1, &tmp, NULL, NULL, NULL);
        if (FD_ISSET(sd, &tmp)) {
            struct sockaddr_storage addr;
            socklen_t addrlen = sizeof(struct sockaddr_storage);
            int cd = accept4(sd, (SA *)&addr, &addrlen, SOCK_NONBLOCK);
            if (cd == -1) {
                if (errno == EAGAIN) {
                    continue;
                } else {
                    perror("accept");
                    exit(1);
                }
            }
            process_request(cd);
        }
    }
}

