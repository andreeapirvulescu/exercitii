#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE  "/server"
#define MODE 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define BUFFER_SIZE 270

int main(int argc, char **argv)
{
    char client_queue[40];
    mqd_t server, client;
    char in_buffer[BUFFER_SIZE];
    char temp_buf[10];

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    sprintf(client_queue, "/client%d", getpid());

    printf("Hello from client %s\n", client_queue);

    if ((client = mq_open(client_queue, O_RDONLY | O_CREAT, MODE, &attr)) == -1)
    {
        perror("Client error in mq_open");
        return 1;
    }

    if ((server = mq_open(SERVER_QUEUE, O_WRONLY)) == -1)
    {
        perror("Client can't open the server queue");
        return 1;
    }

    while (fgets(temp_buf, 2, stdin))
    {

        if (mq_send(server, client_queue, strlen(client_queue) + 1, 0) == -1)
        {
            perror("Client error in mq_send\n");
            continue;
        }

        if (mq_receive(client, in_buffer, BUFFER_SIZE, NULL) == -1)
        {
            perror("Client error in mq_receive");
            return 1;
        }
        printf("Client received token %s\n", in_buffer);
    }


    if (mq_close(client) == -1)
    {
        perror("Client error in mq_close");
        return 1;
    }

    if (mq_unlink(client_queue) == -1)
    {
        perror("Client error in mq_unlink");
        return 1;
    }

    return 0;
}