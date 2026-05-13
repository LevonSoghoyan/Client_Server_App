/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
/*MACROS*/
#define PORT 8080
#define BUFF_SIZE 2048
/*Global variables*/
char Server_Command[BUFF_SIZE];
int server_fd, status;
int client_fd;
struct sockaddr_in address;
socklen_t addrlen = sizeof(address);
int opt = 1;
/********************************
 *@breef


 ******************************/
int Server_Up()
{

    if (listen(server_fd, 3) < 0) {

        printf("listen\n");
    }

    client_fd = accept(server_fd, (struct sockaddr*)&address,&addrlen);
}

int Server_Down()
{
    for(int i = 0; i < 10 ; i++)
        close(client_fd);
    close(server_fd);
}

int *clients()
{
}
void Status()
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, ip, INET_ADDRSTRLEN);
    
    if (server_fd) {
        printf("The server IP is %s \n", ip);
    }
}
int ps()
{
    system("ps");
}

void history()
{

}

void Client_Command()
{
    memset(Server_Command, 0, BUFF_SIZE);
    recv(client_fd , Server_Command, BUFF_SIZE, 0);
    if (strcmp(Server_Command, "shell") == 0) {

        char buffer[BUFF_SIZE];
        char output[BUFF_SIZE];
        memset(output, 0, BUFF_SIZE);
        memset(buffer, 0, BUFF_SIZE);
        char* pcmd  = Server_Command + 6;
        FILE* pf;
        pf = popen(pcmd, "r");
        while(fgets(buffer, sizeof(buffer), pf) != NULL) {
            strcat(output,buffer);
        }
        send(client_fd, output, BUFF_SIZE, 0);
        pclose(pf);
    }
}
int main(int argc, char* argv[])
{
    printf("\nSERVER CLI\n");
    printf("USE help for help message\n");
    /*Creating server socket*/
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket\n");
    }
    /*Adding server config*/
    if (setsockopt(server_fd, SOL_SOCKET,
                SO_REUSEADDR | SO_REUSEPORT, &opt,
                sizeof(opt))) {
        ////
    }    
    char* IP;
    if (argc > 1) {
        IP = argv[1];
    } else {
        IP = "127.0.0.1";
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IP);
    address.sin_port = htons(PORT);

    /* Making server socket to listening mode*/
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {

        printf("bind\n");
    }
    
    Server_Up();
    pid_t pid = fork();
    int UP = 0;
    char Server_Command[BUFF_SIZE];
    
    while(1) {

        while (pid == 0) {
            Client_Command();
            close(server_fd);
        }
        printf("Server>");
        close(client_fd);
        fgets(Server_Command, BUFF_SIZE, stdin);
        Server_Command[strcspn(Server_Command,"\n")] = '\0';

        if (strlen(Server_Command) <= 1)
            continue;

        if (strcmp(Server_Command, "up") == 0) {
            UP = 1;
            Server_Up();
        } else if (strcmp(Server_Command, "down") == 0) {
            Server_Down();
        } else if (strcmp(Server_Command, "clients") == 0) {
            clients();
        } else if (strcmp(Server_Command, "ps") == 0) {
            ps();
        } else if (strcmp(Server_Command, "history") == 0) {
            history();
        } else if (strcmp(Server_Command, "status") == 0) {
            Status();
        } else {
            printf("Usage:\n");
            printf("COMMAND                     DESCRIPTION\n");
            printf("up:                         Start server\n");    
            printf("down:                       Stop server\n");    
            printf("client:                     Show connection list\n");    
            printf("status:                     Show connection status\n");    
            printf("ps:                         Show process list\n");    
            printf("history:                    History\n");    
        }
    }
}
