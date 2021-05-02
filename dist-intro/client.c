#include "csapp.h"


int UDP_clientfd(char *hostname, char *port) {
  struct addrinfo hints, *result;
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_ADDRCONFIG;
  hints.ai_protocol = 0;
  Getaddrinfo(hostname, port, &hints, &result);

  int sfd;
  struct addrinfo *rp;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    if ((sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
      continue;

    if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
      break;
    if (close(sfd) < 0) {
      fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
      return -1;
    }
  }

  if (rp == NULL) {
    fprintf(stderr, "Could not connect\n");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(result);
  return sfd;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <hostname> <port>", argv[0]);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Unimplemented\n");
  return 0;
}
