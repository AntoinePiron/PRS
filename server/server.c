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
#include "server_util.h"

int main(int argc, char **argv)
{

  int sockfd, number_clients;

  sockfd = socket_creation(BASE_PORT, IP);
  number_clients = 0;

  while (1)
  {
    printf("Waiting for connection ...\n");
    three_way_handshake(sockfd, number_clients);
    number_clients++;
    int PID = fork();
    if (PID == 0)
    {
      printf("New client connected\n");
      close(sockfd);
      int new_port = BASE_PORT + number_clients;
      int new_sockfd = socket_creation(new_port, IP);
      exit(0);
    }
  }

  return 0;
}