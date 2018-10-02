#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE "/server"
#define MODE 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define BUFFER_SIZE 270

int main(int argc, char **argv)
{
    mqd_t server, client;
    char in_buffer[BUFFER_SIZE];
    char out_buffer[BUFFER_SIZE];
    int token = 1;

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    printf("Hello from server\n");

    if ((server = mq_open(SERVER_QUEUE, O_RDONLY | O_CREAT, MODE, &attr)) == -1)
    {
        perror ("Server: mq_open (server)");
        return 1;
    }

    while (1) {
        if (mq_receive(server, in_buffer, BUFFER_SIZE, NULL) == -1)
        {
            perror("Server error in mq_receive");
            return 1;
        }

        printf("Server received a request\n");

        if ((client = mq_open(in_buffer, O_WRONLY)) == 1)
        {
            perror("Server can't open the client queue\n");
            continue;
        }

        sprintf (out_buffer, "%i", token);

        if (mq_send(client, out_buffer, strlen (out_buffer) + 1, 0) == -1)
        {
            perror("Server error in mq_send\n");
            continue;
        }

        printf("Server send token %i to the client %s\n", token, in_buffer);
        token++;
    }

    if (mq_close(server) == -1)
    {
        perror("Server error in mq_close");
        return 1;
    }

    if (mq_unlink(SERVER_QUEUE) == -1)
    {
        perror("Server error in mq_unlink");
        return 1;
    }

    return 0;
}