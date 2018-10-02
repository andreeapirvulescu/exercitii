#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define BUFFER_SIZE 256
   
int main(int argc, char *argv[]) 
{ 
    struct sockaddr_in address; 
    int client_fd, valread, port, file_size, fd; 
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0}, file_name[20] = {0};
    char *not_found_message = "File not found";
    int remain_data = 0, received_bytes = 0;

    if (argc < 4)
	{
		printf("Usage: %s server_ip server_port file\n", argv[0]);
		return 1;
	}

	port = atoi(argv[2]);

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        perror("Error opening socket");
        return 1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)  
    { 
        printf("Invalid server address\n"); 
        return 1; 
    }   
   
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("Connection Failed\n"); 
        return 1; 
    }

    send(client_fd, argv[3], strlen(argv[3]), 0); 

    received_bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);

    if (strcmp(buffer, not_found_message) == 0)
    {
    	printf("There is no file with that name on the server\n");
        return 0;
    } else {
    	file_size = atoi(buffer);
    	sprintf(file_name, "client%d", getpid());
    	if ((fd = open(file_name, O_WRONLY | O_CREAT, 0666)) == -1)
    	{
    		perror("Error with opening the file");
    	}

        remain_data = file_size;
        char *str = NULL;
        str = strstr(buffer, "\t");
        int bytes_written = 0;
        if (str)
        {
            bytes_written = write(fd, str+1, received_bytes-abs(buffer-str-1));
            remain_data -= bytes_written;
        }
    	
        memset(buffer, 0, BUFFER_SIZE);

        while (remain_data > 0)
        {
            received_bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (received_bytes < 0)
            {
                perror("Error in recv");
            }
            write(fd, buffer, received_bytes);
            remain_data -= received_bytes;
            memset(buffer, 0, BUFFER_SIZE);
        }

    }

   	close(fd);
   	close(client_fd);

    return 0; 
}