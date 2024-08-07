/*
gcc -o Server Server.c -lrt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_SIZE 1024
#define QUEUE_PERMISSIONS 0660

typedef struct 
{
    char client_queue_name[64];
} request_t;

typedef struct 
{
    int token_number;
} response_t;

int main() 
{
    mqd_t server_queue, client_queue;
    struct mq_attr attr;
    request_t request;
    response_t response;
    int token = 1;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(request_t);
    attr.mq_curmsgs = 0;

    server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if (server_queue == (mqd_t)-1) 
    {
        perror("Server: mq_open (server)");
        exit(1);
    }

    while (1) 
    {
        if (mq_receive(server_queue, (char*)&request, sizeof(request_t), NULL) == -1) 
        {
            perror("Server: mq_receive");
            exit(1);
        }

        printf("Server: Received request from client queue: %s\n", request.client_queue_name);

        response.token_number = token++;

        client_queue = mq_open(request.client_queue_name, O_WRONLY);
        if (client_queue == (mqd_t)-1) 
        {
            perror("Server: mq_open (client)");
            continue;
        }

        if (mq_send(client_queue, (char*)&response, sizeof(response_t), 0) == -1) 
        {
            perror("Server: mq_send");
            continue;
        }

        printf("Server: Sent token %d to client queue: %s\n", response.token_number, request.client_queue_name);

        mq_close(client_queue);
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}
