/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

/*MACROS*/
#define PORT 8080
#define BUFF_SIZE 2048

#define PROMPT "Client> "
/*Global variables*/
char buf[BUFF_SIZE];
int client_fd = -1;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

void Status()
{
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in addr;

    if (getpeername(client_fd, (struct sockaddr *)&addr, &addr_len) == 0)
        inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    if (client_fd && inet_pton(AF_INET, ip, &(addr.sin_addr)) == 1) {
        printf("Client connected to server with the IP address %s \n", ip);
    } else {
        printf("Client disconnected \n");
    }
}

void Connect(char* IP)
{
    /*Creating Socket*/
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: %s \n",strerror(errno));
    }
    if (inet_pton(AF_INET, IP, &server_addr.sin_addr) < 0) {
        printf("Error: %s \n",strerror(errno));
    }

    /*Connecting client whit server*/
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: %s\n",strerror(errno));
    }
    Status();  
    return;
}

void Shell(char* command)
{

    if (command == "disconnect")
    {
        send(client_fd, command, BUFF_SIZE, 0);
        return;
    }
    send(client_fd, command, BUFF_SIZE, 0);
    memset(buf, 0, BUFF_SIZE);
    
    recv(client_fd, buf, BUFF_SIZE, 0);
    while(strcmp(buf,"NULL") != 0)
    {
        printf("%s", buf);
        memset(buf, 0, BUFF_SIZE);
        recv(client_fd, buf, BUFF_SIZE, 0);
    }
    return;
}

void D_connect()
{
    char* command = "disconnect";
    Shell(command);
    printf("Disconnected\n");
    close(client_fd);
    client_fd = -1;
}

int main(int argc, char* argv[])
{
    printf("\nCLIENT CLI\n");

    /*Adding server configurations*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    char buf[BUFF_SIZE];
    while(1) {
        char* input = readline(PROMPT);
        strncpy(buf, input, BUFF_SIZE -1);   
        add_history(input);

        if (strlen(buf) <= 1)
            continue;

        char *token;
        token = strtok(buf, " ");
        token[strcspn(token,"\n")] = '\0';
        
        if (strcmp(token, "connect") == 0) {
            Connect((buf + strlen(token) + 1));
        } else if (!client_fd) {
            printf("Client disconnected\n");
        } else if (strcmp(token, "shell") == 0) {
            Shell(buf);
        } else if (strcmp(token, "disconnect") == 0) {
            D_connect();
        } else if (strcmp(token, "status") == 0) {
            Status();
        } else {
            printf("Usage:\n");
            printf("COMMAND                     DESCRIPTION\n");
            printf("connect  <IP>               connect to server by <IP>\n");
            printf("shell <command>             run <command> in server terminal\n");
            printf("disconnect                  disconnect client \n");
            printf("status                      show connection status\n");
        }
    }
}
