#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8000
#define lli long long int

lli N = 32768;
void sendfile(int fp, int sockfd);
int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Connected to Client...\n\n");
    // valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer
    // printf("%s\n",buffer);
    char cwd[1024];
    getcwd(cwd,1024);
    // printf("send cwd active\n");
    send(new_socket , cwd , strlen(cwd) , 0 );  // use sendto() and recvfrom() for DGRAM
    // printf("send cwd over\n");
    // printf("Hello message sent\n");

    while(1>0)
    {
        // printf("A\n");
        char filename[1024];
        // printf("B\n");
        // printf("read filename active\n");
        valread = read(new_socket, filename, sizeof(filename));
        // printf("read filename over\n");

        if(strcmp(filename,"-1")==0)
            break;
        // printf("D\n");
        int fp = open(filename,O_RDONLY);
        if(fp < 0)
            continue;
        // printf("E\n");

        sendfile(fp,new_socket);
        // printf("F\n");
        // printf("G\n");
        // fclose(fp);
    }
    printf("Ending Session..!!\n");

    return 0;
}

void sendfile(int fp, int sockfd) 
{
    lli n; 
    char sendline[N]; 
    memset(sendline, 0, N);
    // printf("Received request to download\n");
    while ((n = read(fp, sendline, N)) > 0) 
    {
        if (n == -1)
        {
            perror("Read File Error");
            exit(1);
        }
        // printf("sending data = %s\n", sendline);
        // printf("send data active\n");
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        // printf("send data over\n");
        memset(sendline, 0, N);
    }
    sleep(1);
    // printf("send empty active\n");
    send(sockfd, "umangover", sizeof("umangover"), 0);
    // printf("send empty over\n");
    // printf("Exiting sendfile\n");
}
