/*
gcc -o Client Client.c -lrt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
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
    char client_queue_name[64];

    snprintf(client_queue_name, 64, "/client_queue_%d", getpid());

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(response_t);
    attr.mq_curmsgs = 0;

    client_queue = mq_open(client_queue_name, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if (client_queue == (mqd_t)-1) 
    {
        perror("Client: mq_open (client)");
        exit(1);
    }

    server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_queue == (mqd_t)-1) 
    {
        perror("Client: mq_open (server)");
        exit(1);
    }

    strncpy(request.client_queue_name, client_queue_name, 64);

    if (mq_send(server_queue, (char*)&request, sizeof(request_t), 0) == -1) 
    {
        perror("Client: mq_send");
        exit(1);
    }

    printf("Client: Sent request, waiting for response...\n");

    if (mq_receive(client_queue, (char*)&response, sizeof(response_t), NULL) == -1) 
    {
        perror("Client: mq_receive");
        exit(1);
    }

    printf("Client: Received token number: %d\n", response.token_number);

    mq_close(client_queue);
    mq_unlink(client_queue_name);
    mq_close(server_queue);

    return 0;
}
