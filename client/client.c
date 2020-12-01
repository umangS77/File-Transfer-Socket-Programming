// Client side C/C++ program to demonstrate Socket programming
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
#define PERMS S_IRUSR | S_IWUSR

lli N = 32768;
void receivefile(int sockfd, int fp, lli deno);
int main()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected to Server...\n\n");
    char server_wd[1024];
    // printf("read serverdir active\n");
    valread = read( sock , server_wd, 1024);  // receive message back from server, into the buffer
    // printf("read serverdir over\n");
    // printf("%s\n",server_wd);
    lli v = strlen(server_wd);
    server_wd[v]='/';
    char client_wd[1024];
    getcwd(client_wd,1024);
    lli xv = strlen(client_wd);
    client_wd[xv]='/';
    xv++;
    v++;

    while(1>0)
    {
        char arr[100][1000];
        memset(arr,0,sizeof(arr));
        char inp[10000];
        printf("client> ");
        fgets(inp,10000,stdin);
        // printf("input = %s\n",inp);
        lli l = strlen(inp);
        l--;
        lli cnt=0,j=0;
        for(lli i=0;i<=l;i++)
        {
            if(inp[i]==' ' || i==l)
            {
                cnt++;
                j=0;
            }
            else
            {
                arr[cnt][j]=inp[i];
                j++;
            }
        }


        if(cnt == 1 && strcmp(arr[0],"exit") == 0)
        {
            send(sock, "-1", sizeof("-1"), 0);
            printf("GoodBye..!!\n\n");
            break;
        }

        else if((strcmp(arr[0],"get")!=0 && strcmp(arr[0],"exit")!=0) || cnt <=1 || (strcmp(arr[0],"exit")==0) && cnt!=1)
        {
            printf("Usage:\n\tto download files: get <file1> <file2> ......\n\tto exit: exit\n");
            continue;
        }

        for(j=1;j<cnt;j++)
        {
            // printf("arr[j] = %s\n",arr[j]);
            if(strcmp(arr[j],"")==0)
            	continue;
            char source[1024];
            memset(source, 0, 1024);
            // printf("\ninitial source = %s\n",source);       
            strcpy(source,server_wd);
            for(lli k=0;k<strlen(arr[j]);k++)
            {
                source[k+v]=arr[j][k];
            }
            // printf("\nsource = %s\n",source);
            int fl = open(source, O_RDONLY);
            if(fl < 0)
            {
                printf("File Not Found: %s\n\n",source);
                continue;
            }
            lli size=0;
            char dest[1024];
            memset(dest, 0, 1024);
            strcpy(dest,client_wd);
            for(lli k=0;k<strlen(arr[j]);k++)
            {
                dest[k+xv]=arr[j][k];
            }
            // printf("\ndestination = %s\n",dest);
            lli index=0;
            char buff[N];

            int file=open(source,O_RDONLY);
            // fseek(file, 0, SEEK_END);
            // size = ftell(file);
            // fseek(file, 0, SEEK_SET);
            // int chch;
            // while(1>0)
            // {
            // 	chch = fgetc(file);
            // 	if(chch == EOF)
            // 		break;
            // 	size++;
            // }
            size = lseek(file,0,SEEK_END);
            lseek(file,0,SEEK_SET);
            // printf("size= %lld\n",sizes);


            int fl2 = open(dest, __O_LARGEFILE | O_CREAT | O_WRONLY | O_TRUNC, PERMS);
            if(fl2 < 0)
            {
                perror("File Unable to Download");
                break;
            }
            lli deno = size/N + 1;
            // printf("send filename active\n");
            send(sock, source, sizeof(source), 0);
            // printf("send filename over\n");
            receivefile(sock, fl2, deno);
            // send(sock, "umangover", sizeof("umangover"), 0);
            printf("\n%s downloaded\n\n",arr[j]);
            close(fl2);
            close(file);
            
        }
    }
    return 0;
}


void receivefile(int sockfd, int fp, lli deno)
{
    ssize_t n=1;
    char buff[N];
    memset(buff,0,N);
    lli cnt2=1;
    // printf("sending sources\n");
    while (n > 0) 
    {
        // printf("receive data active\n");
        n = recv(sockfd, buff, N, 0);
        // printf("receive data over\n");
        if(n<=0 || strcmp(buff,"umangover")==0)
            break;
        // printf("received data = %s\nlen= %lld\n",buff,strlen(buff));
        
        // if(strcmp(buff,"umangover")==0)
        //     break;
        double ans = cnt2*100.00 / deno;
        if(ans>100)
            ans=100;
        printf("\r%0.2f %% downloaded....",ans);
        cnt2++;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }
        
        if (write(fp, buff, n) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, N);
    }
    // printf("Exiting receivefile\n");
}
