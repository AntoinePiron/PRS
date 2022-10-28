#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../constants.h"
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

  int new_port;
  new_port = three_way_handshake(sockfd, addr);
  close(sockfd);

  int new_socket = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in second_addr;
  memset(&second_addr, '\0', sizeof(second_addr));
  second_addr.sin_family = AF_INET;
  second_addr.sin_port = htons(new_port);
  second_addr.sin_addr.s_addr = inet_addr(ip);

  ask_file(new_socket, second_addr);

  return 0;
}
