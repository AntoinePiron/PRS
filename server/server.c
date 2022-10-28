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

/* Commande pou génerer un fichier de test
 * dd if=/dev/urandom of=fichier count=8
 */

int main(int argc, char **argv)
{

  int number_clients, prot_sock, comm_sock, maxfdp1, nready;
  fd_set rset;

  number_clients = 0;
  prot_sock = socket_creation(BASE_PORT, IP);
  comm_sock = socket_creation(COMMUNICATION_PORT, IP);

  // clear the descriptor set
  FD_ZERO(&rset);
  // get maxfd
  maxfdp1 = max(prot_sock, comm_sock) + 1;

  while (1)
  {
    // set listenfd and udpfd in readset
    FD_SET(prot_sock, &rset);
    FD_SET(comm_sock, &rset);
    printf("Waiting for connection...\n");

    nready = select(maxfdp1, &rset, NULL, NULL, NULL);

    // if tcp socket is readable then handle
    // it by accepting the connection
    if (FD_ISSET(prot_sock, &rset))
    {
      three_way_handshake(prot_sock, number_clients);
      number_clients++;
    }
    // if udp socket is readable receive the message.
    if (FD_ISSET(comm_sock, &rset))
    {
      handle_file(comm_sock);
    }
  }
  return 0;
}