#include "server_util.h"
#include "../constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// File function and bzero
#include <fcntl.h>
#include <unistd.h>

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
    int valid = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &valid, sizeof(int));
    return sockfd;
}

void three_way_handshake(int sockfd, int client_num)
{
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;
    struct sockaddr_in client_addr;

    bzero(&buffer, BUFFER_SIZE);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("*** Starting three way handshake *** \n\n");
    printf("[+]Data recv: %s\n", buffer);
    if (strncmp(buffer, "SYN", 3) != 0)
    {
        printf("SYN not received \n");
        exit(EXIT_FAILURE);
    }

    bzero(&buffer, BUFFER_SIZE);
    snprintf(buffer, 13, "SYN ACK_%d", COMMUNICATION_PORT);
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    printf("[+]Data send: %s\n", buffer);

    bzero(&buffer, BUFFER_SIZE);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);
    if (strncmp(buffer, "ACK", 3) != 0)
    {
        printf("ACK not received \n");
        exit(EXIT_FAILURE);
    }
    printf("\n*** end of three way handshake *** \n\n");
}

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

void handle_file(int sockfd)
{
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;
    struct sockaddr_in client_addr;
    int fd;
    long int n;
    off_t m, count = 0;

    bzero(&buffer, BUFFER_SIZE);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("\t[+]Data recv: %s\n", buffer);
    char *filename = strtok(buffer, " ");
    printf("\t[+]File name: %s\n", filename);
    char *complete_filename = malloc(strlen(filename) + 7);
    sprintf(complete_filename, "server/%s", filename);

    if ((fd = open(complete_filename, 0600)) == -1)
    {
        perror("\t[-]Open fail");
        exit(EXIT_FAILURE);
    }

    int segment = 0;
    do
    {
        bzero(&buffer, BUFFER_SIZE);
        segment++;
        n = read(fd, buffer, BUFFER_SIZE - 6);
        // add segment number at the end of the buffer
        sprintf(buffer + strlen(buffer), "%06d", segment);

        if (n == -1)
        {
            perror("read fails");
            exit(EXIT_FAILURE);
        }
        m = (sockfd, buffer, n, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (m == -1)
        {
            perror("send error");
            exit(EXIT_FAILURE);
        }
        count += m;
        printf("\t[+]Data send\n");
    } while (n);

    exit(0);
}