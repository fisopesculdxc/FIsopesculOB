/*
gcc -o Client Client.c
./Client localhost port_number filename
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 255

void error(const char* msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[])
{
    int sockfd, portno, n;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if(argc < 4)
    {
        fprintf(stderr, "Usage %s hostname port filename\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        error("Error opening socket.");
    }
     
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host.\n");
        exit(1);
    }

    memset((char*) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char*) &serv_addr.sin_addr.s_addr, (char*) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Connection failed.");
    }

    memset(buffer, 0, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "%s\n", argv[3]);
    n = write(sockfd, buffer, strlen(buffer));
    if(n < 0)
    {
        error("Error on writing.");
    }

    memset(buffer, 0, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE);
    if(n < 0)
    {
        error("Error on reading.");
    }

    if (strncmp(buffer, "File not found", 14) == 0) 
    {
        printf("Server: %s\n", buffer);
    } 
    else 
    {
        fp = fopen(argv[3], "w");
        if (fp == NULL)
        {
            error("Error opening file to save.");
        }

        fwrite(buffer, sizeof(char), n, fp);
        while ((n = read(sockfd, buffer, BUFFER_SIZE)) > 0)
        {
            if(n < 0)
            {
                error("Error on reading.");
            }
            fwrite(buffer, sizeof(char), n, fp);
        }

        printf("File received and saved as %s\n", argv[3]);
        fclose(fp);
    }

    close(sockfd);

    return 0;
}
