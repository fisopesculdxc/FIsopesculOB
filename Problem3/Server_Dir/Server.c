/*
gcc -o Server Server.c -lpthread
./Server port_number
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 255

void error(const char* msg)
{
    perror(msg);
    exit(1);
}

void* handle_client(void* arg)
{
    int newsockfd = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    int n;

    memset(buffer, 0, BUFFER_SIZE);
    n = read(newsockfd, buffer, BUFFER_SIZE);
    if(n < 0)
    {
        error("Error on reading.");
    }

    buffer[strcspn(buffer, "\n")] = 0;

    FILE *fp = fopen(buffer, "r");
    if (fp == NULL)
    {
        n = write(newsockfd, "File not found\n", 15);
        if(n < 0)
        {
            error("Error on writing.");
        }
    }
    else
    {
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
        {
            n = write(newsockfd, buffer, strlen(buffer));
            if(n < 0)
            {
                error("Error on writing.");
            }
        }
        fclose(fp);
    }

    close(newsockfd);
    return NULL;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Port not provided. Program terminated\n");
        exit(1);
    }

    int sockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        error("Error opening socket.");
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding failed.");
    }
    
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while(1)
    {
        int* newsockfd = malloc(sizeof(int));
        if(newsockfd == NULL)
        {
            error("Error on memory allocation.");
        }

        *newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);

        if(*newsockfd < 0)
        {
            error("Error on accept.");
        }

        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, handle_client, newsockfd) != 0)
        {
            error("Error creating thread.");
        }
        pthread_detach(thread_id);
    }

    close(sockfd);

    return 0;
}
