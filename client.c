#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "constants.h"
#include "client_util.h"

int main(int argc, char **argv)
{

  char *ip = IP;

  int sockfd;
  struct sockaddr_in addr;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(BASE_PORT);
  addr.sin_addr.s_addr = inet_addr(ip);

  three_way_handshake(sockfd, addr);

  return 0;
}
