/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
char buff[1024];
int client_fd, status;
#define PORT 8080
int Connect(char* IP)
{

    /*Creating Socket*/
    struct sockaddr_in server_addr;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socet crt\n");
    }
    /*Adding server configurations*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {

        printf("ip_addr   \n");
    }
    /*Connecting client whit server*/
    if ((status = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {

        printf("connection   \n");
    }
    send(client_fd, "CLIENT TO SERVER", 16, 0);
    read(client_fd, buff,1024);
    printf("%s\n",buff);
    close(client_fd);
}

int D_connect()
{

}

char* Shell(char* command)
{

}

void Status()
{

}

int main(int argc, char* argv[])
{
    Connect("127.0.0.1");
}

