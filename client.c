/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
/*MACROS*/
#define PORT 8080
#define BUFF_SIZE 2048
/*Global variables*/
char buf[BUFF_SIZE];
int client_fd, status;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);
void Connect(char* IP)
{
    if (inet_pton(AF_INET, IP, &server_addr.sin_addr) <= 0) {

        printf("ip_addr   \n");
    }
    /*Connecting client whit server*/
    if ((status = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {

        printf("connection   \n");
    }
    return;
}

int D_connect()
{
    close(client_fd);
}

void Shell(char* command)
{

    send(client_fd, command, BUFF_SIZE, 0);
    memset(buf, 0, BUFF_SIZE);
    recv(client_fd, buf, BUFF_SIZE, 0);
    printf("%s", buf);
    return;
}

void Status()
{
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in addr;
    if (getpeername(client_fd, (struct sockaddr *)&addr, &addr_len) == 0)
        inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    if (client_fd) {
        printf("The client connected to the server with the IP address %s \n", ip);
    } else {
        printf("Client disconnected \n");
    }
}

int main(int argc, char* argv[])
{
    printf("\nCLIENT CLI\n");
    printf("USE help for help massage\n");
    /*Creating Socket*/
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socet crt\n");
    }
    /*Adding server configurations*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    while(1) {
        printf("Client >");
        char buf[BUFF_SIZE];
        fgets(buf, BUFF_SIZE, stdin);

        int i = 0;

        if (strlen(buf) <= 1)
            continue;

        char *token;
        token = strtok(buf, " ");
        token[strcspn(token,"\n")] = '\0';
        if (strcmp(token, "connect") == 0) {
            Connect((buf+strlen(token) + 1));
        } else if (strcmp(token, "shell") == 0) {
            Shell(buf);
        } else if (strcmp(token, "disconnect") == 0) {
            break;
        } else if (strcmp(token, "status") == 0) {
            Status();
        } else {
            printf("Usage:\n");
            printf("COMMAND                     DESCRIPTION\n");
            printf("connect  <IP>:              connect to server by <IP>\n");    
            printf("shell <command>:            run <command> in server terminal\n");    
            printf("disconnect:                 disconnect client \n");    
            printf("status:                     show connection status\n");    
        }
    }
}
