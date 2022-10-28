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
    char data_buffer[BUFFER_SIZE - 6];
    socklen_t addr_size;
    int fd;
    long int n;
    off_t count = 0;

    bzero(&buffer, BUFFER_SIZE);
    addr_size = sizeof(addr);
    strcpy(buffer, "fichier.jpg");
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+]Data send: %s\n", buffer);
    char *filename = "client/fichier.jpg";

    if ((fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0600)) == -1)
    {
        perror("open fail");
        exit(EXIT_FAILURE);
    }

    do
    {
        bzero(&buffer, BUFFER_SIZE);
        n = recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addr_size);
        printf("%lld of data received \n", n);
        // get segment number by getting last 6 characters from the buffer
        char *segment = buffer + strlen(buffer) - 6;
        printf("[+]Segment number: %s \n", segment);
        // remove 6 last characters from the buffer
        buffer[strlen(buffer) - 7] = '\0';

        if (n == -1)
        {
            perror("read fails");
            exit(EXIT_FAILURE);
        }
        count += n;
        write(fd, buffer, sizeof(buffer));
    } while (n);
}