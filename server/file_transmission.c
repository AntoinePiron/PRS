#include "file_transmission.h"
#include "../constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

char buffer[BUFFER_SIZE];
socklen_t addr_size;
struct sockaddr_in client_addr;
FILE *fd;
long int n;
off_t m;

int segment = 1;
int last_segment = 0;
int window_size = 1;

struct timeval tv, start, end;

int global_sockfd;
pthread_t thread_id;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

void *handle_file_ack(void *arg)
{
    char recepBuffer[BUFFER_SIZE];
    while (1)
    {
        bzero(recepBuffer, BUFFER_SIZE);
        recvfrom(global_sockfd, recepBuffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
        char segment_str[SEGMENT_NUMBER_LENGTH + 1];
        char *segment_recv = strtok(recepBuffer, "_");
        int recep_segment = atoi(segment_recv);
        printf("\t[+]ACK recv: %d\n", recep_segment);
        if (recep_segment < last_segment + 1)
        {
            printf("\t[+]ACK OK\n");
            pthread_mutex_lock(&mutex);
            window_size += 2;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            printf("\t[+]ACK ERROR\n");
        }
        printf("\tWindow size: %d\n", window_size);
    }
    return NULL;
}

void handle_file(int sockfd)
{
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    global_sockfd = sockfd;

    bzero(buffer, BUFFER_SIZE);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("\t[+]Data recv: %s\n", buffer);
    char *filename = strtok(buffer, " ");
    printf("\t[+]File name: %s\n", filename);
    char *complete_filename = malloc(strlen(filename) + 7);
    sprintf(complete_filename, "server/%s", filename);

    if ((fd = fopen(complete_filename, "r")) == NULL)
    {
        perror("\t[-]Open fail");
        exit(EXIT_FAILURE);
    }
    pthread_create(&thread_id, NULL, handle_file_ack, NULL);
    do
    {

        while (1)
        {
            if (window_size != 0)
                bzero(buffer, BUFFER_SIZE);
            sprintf(buffer, "%06d", segment);
            fseek(fd, (segment - 1) * (BUFFER_SIZE - SEGMENT_NUMBER_LENGTH), SEEK_SET);
            n = fread(buffer + SEGMENT_NUMBER_LENGTH, 1, BUFFER_SIZE - SEGMENT_NUMBER_LENGTH, fd);

            if (n == -1)
            {
                perror("read fails");
                exit(EXIT_FAILURE);
            }
            if (n == 0)
                break;

            last_segment = segment;
            while (window_size == 0)
            {
                continue;
            }
            m = sendto(sockfd, buffer, n + SEGMENT_NUMBER_LENGTH, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            if (m == -1)
            {
                perror("send error");
                exit(EXIT_FAILURE);
            }
            printf("\t[+]bytes send %lld\n", m);
            segment++;
            pthread_mutex_lock(&mutex);
            window_size--;
            pthread_mutex_unlock(&mutex);
        }
        last_segment = segment - 1;
    } while (n == BUFFER_SIZE - SEGMENT_NUMBER_LENGTH);
    fclose(fd);
    exit(0);
}