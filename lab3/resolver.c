#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

void resolve(char*);

#define EXIT_USAGE_ERROR 1
#define MAX_LEN 64

int main(int argc, char *argv[]) {
  int i;

  if (argc < 2) {
    fprintf(stderr,"usage: %s hostname\n", argv[0]);
    return EXIT_USAGE_ERROR;
  }

  for (i=1; i<argc; i++) {
    printf("resolving %s\n", argv[i]);
    resolve(argv[i]);
    printf("\n\n\n");
  }

  return EXIT_SUCCESS;
}

void resolve(char *host) {
  int status;
  struct addrinfo hints, *res, *p;
  char ipstr[MAX_LEN];		/* IP addr as string */

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  if ((status = getaddrinfo(host, NULL, &hints, &res)) != 0) {
    fprintf(stderr, "error resolving %s: %s\n", host, gai_strerror(status));
    return;
  }

  for(p = res;p != NULL; p = p->ai_next) {
    void *addr;

    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
    }

    inet_ntop(p->ai_family, addr, ipstr, MAX_LEN-1);
    printf("    %s\n", ipstr);
  }

  freeaddrinfo(res);
}
