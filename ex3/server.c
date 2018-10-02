#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include <string.h> 

#define MAX_CLIENTS 20
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) 
{ 
    int server_fd, new_socket, port, fd, i = 0; 
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 
    char buffer[BUFFER_SIZE] = {0}; 
    char *not_found_message = "File not found";
    struct stat file_stat;
    char file_size[256] = {0};
    int remain_data, bytes_read = 0, bytes_written = 0;
    fd_set read_fds, active_fds;

    if (argc < 2)
	{
		printf("Usage: %s port \n", argv[0]);
		return 1;
	}

	port = atoi(argv[1]);

    FD_ZERO(&active_fds);
       
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("Error opening socket:");
        return 1; 
    } 
       
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 
       
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    { 
        perror("ERROR in bind"); 
        return 1; 
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) 
    { 
        perror("Error in listen"); 
        return 1; 
    }


    FD_SET(server_fd, &active_fds);

    while (1)
    {
        read_fds = active_fds;
        if (select (FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
        {
          perror("Error in select");
          return 1;
        }
        for (i = 0; i < FD_SETSIZE; ++i)
        {
            if (FD_ISSET (i, &read_fds))
            {
                if (i == server_fd)
                {
                    /* Connection request on original socket. */
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                            (socklen_t*)&addrlen)) < 0) 
                    { 
                        perror("Error in accept"); 
                        return 1; 
                    } 
                    FD_SET(new_socket, &active_fds);
                } else {
                    /* Data arriving on an already-connected socket. */
                    recv(new_socket, buffer, BUFFER_SIZE, 0);
                    if (stat(buffer, &file_stat))
                    {
                        send(new_socket, not_found_message, strlen(not_found_message), 0); 
                    } else {
                        if ((fd = open(buffer, O_RDONLY)) == -1)
                        {
                            perror("Error with opening the file");
                        }
                        sprintf(file_size, "%ld\t", file_stat.st_size);
                        send(new_socket, file_size, strlen(file_size), 0);
                        remain_data = file_stat.st_size;

                        memset(buffer, 0, BUFFER_SIZE);

                        while(remain_data > 0)
                        {
                            bytes_read =  read(fd, buffer, BUFFER_SIZE);
                            if (bytes_read == 0)
                            {
                                break;
                            }
                            if (bytes_read < 0)
                            {
                                perror("Error during read");
                            }
                            while (bytes_read > 0)
                            {
                                bytes_written = send(new_socket, buffer, bytes_read, 0);
                                remain_data -= bytes_written;
                                if (bytes_written < 0)
                                {
                                    perror("Error during send");
                                }
                                bytes_read -= bytes_written;
                                memset(buffer, 0, BUFFER_SIZE);
                            }
                        }
                        close(fd);
                        close(i);
                        FD_CLR (i, &active_fds);
                    }
                }
            }
        }
    }

    return 0; 
}