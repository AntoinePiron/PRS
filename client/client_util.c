#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "../constants.h"
#include "client_util.h"

int three_way_handshake(int sockfd, struct sockaddr_in addr)
{
    printf("\n*** Starting three way handshake *** \n\n");
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;

    bzero(&buffer, BUFFER_SIZE);
    strcpy(buffer, "SYN");
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+]Data send: %s\n", buffer);

    bzero(&buffer, BUFFER_SIZE);
    addr_size = sizeof(addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);
    if (strncmp(buffer, "SYN ACK", 7) != 0)
    {
        printf("SYN ACK not received \n");
        exit(EXIT_FAILURE);
    }
    // get port number by splitting the string with _
    char *port = strtok(buffer, "_");
    port = strtok(NULL, "_");
    int new_port = atoi(port);

    bzero(&buffer, BUFFER_SIZE);
    strcpy(buffer, "ACK");
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+]Data send: %s\n", buffer);

    printf("\n*** end of three way handshake ***\n\n");

    return new_port;
}

void ask_file(int sockfd, struct sockaddr_in addr)
{
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;
    FILE *fd;
    long int n;
    off_t m;

    bzero(buffer, BUFFER_SIZE);
    addr_size = sizeof(addr);
    strcpy(buffer, "fichier.jpg");
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+]Data send: %s\n", buffer);
    char *filename = "client/fichier.jpg";

    if ((fd = fopen(filename, "w")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    do
    {
        bzero(buffer, BUFFER_SIZE);
        n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addr_size);
        // get segment number by getting last 6 characters from the buffer
        char segment[7];
        bzero(segment, 7);
        memcpy(segment, buffer, 6);
        printf("[+]Segment number: %s \n", segment);
        if (n == -1)
        {
            perror("read fails");
            exit(EXIT_FAILURE);
        }
        m = fwrite(buffer + 6, 1, n - 6, fd);
        printf("[+]%lld bytes written to file \n", m);
        // ACK segment
        sendto(sockfd, segment, 6, 0, (struct sockaddr *)&addr, sizeof(addr));
        printf("[+]ACK sent for segment %s \n", segment);

    } while (n == BUFFER_SIZE);

    fclose(fd);
}