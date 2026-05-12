/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
/*MACROS*/
#define PORT 8080
#define BUFF_SIZE 2048
/*Global variables*/
char buf[1024];
int client_fd, status;
struct sockaddr_in server_addr;

void Connect(char* IP)
{

    if (inet_pton(AF_INET, IP, &server_addr.sin_addr) <= 0) {

        printf("ip_addr   \n");
    }
    /*Connecting client whit server*/
    if ((status = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {

        printf("connection   \n");
    }
    send(client_fd, "CLIENT TO SERVER", 16, 0);
    read(client_fd, buf,1024);
    printf("%s\n",buf);
    return;
}

int D_connect()
{
    close(client_fd);
}

char* Shell(char* command)
{
    send(client_fd, "CLIENT TO SERVER", 16, 0);
    read(client_fd, buf,1024);
    printf("%s\n",buf);
}

void Status()
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_addr.sin_addr, ip, INET_ADDRSTRLEN);
    if(client_fd) {
        printf("Client whit IP %s is connected", ip);
    } else {
        printf("Client whit IP %s is disconnected", ip);
    }
}

int main(int argc, char* argv[])
{
    /*Creating Socket*/
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socet crt\n");
    }
    /*Adding server configurations*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    while(1) {
        char buff[BUFF_SIZE];
        fgets(buff, BUFF_SIZE, stdin);
        buff[strcspn(buff,"\n")] = '\0';
        if (strlen(buff) < 1)
            continue;
        char *token;
        token = strtok(buff, " ");
        char* str_command[5];
        int count = 1;
        while (token != NULL) {
            str_command[count++] = token;
            token = strtok(NULL, " ");
        }
        printf("\ncommand\n %s",buff);
        if (count > 2 && strcmp(str_command[1], "connect") == 0) {
            Connect(str_command[2]);
        } else if (count > 2 && strcmp(str_command[1], "shell") == 0) {
            Shell(str_command[2]);
        } else if (count > 1 && strcmp(str_command[1], "disconnect") == 0) {
            D_connect();
        } else if (count > 1 && strcmp(str_command[1], "status") == 0) {
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

