#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../constants.h"
#include "server_util.h"

int main(int argc, char **argv)
{

  int sockfd;

  sockfd = socket_creation(BASE_PORT, IP);

  three_way_handshake(sockfd);

  return 0;
}