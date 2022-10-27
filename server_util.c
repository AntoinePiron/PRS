#include "server_util.h"
#include "constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int socket_creation(int port, char *adress)
{
    char *ip = adress;

    int sockfd;
    struct sockaddr_in server_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("[-]socket error");
        exit(1);
    }

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]bind error");
        exit(1);
    }
    return sockfd;
}

void three_way_handshake(int sockfd)
{
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;
    struct sockaddr_in client_addr;

    bzero(buffer, BUFFER_SIZE);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);
    if (strncmp(buffer, "SYN", 3) != 0)
    {
        printf("SYN not received \n");
        exit(EXIT_FAILURE);
    }

    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, "SYN ACK");
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    printf("[+]Data send: %s\n", buffer);

    bzero(buffer, BUFFER_SIZE);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);
    if (strncmp(buffer, "ACK", 3) != 0)
    {
        printf("ACK not received \n");
        exit(EXIT_FAILURE);
    }
}
