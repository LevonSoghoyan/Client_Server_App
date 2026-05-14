/*INCLUDES*/
#include <time.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
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
#define CLIENTS_COUNT 10

#define PROMPT "Server> "

/*Global variables*/
char Server_Command[BUFF_SIZE];
char PS[BUFF_SIZE];
struct sockaddr_in address;
socklen_t addrlen = sizeof(address);
int server_fd, status;
int opt = 1;
int client_fd;

/*Struct*/
typedef struct Clients {
    int socket;
    char time[30];
} Tclients;

Tclients *Clients;

void Server_Up()
{
    printf("Server is running\n");
}

void Server_Down()
{
    exit(0);
}

void clients()
{
    for(int i = 0; i < CLIENTS_COUNT; i++) {
        if (Clients[i].socket != 0) {
            printf("Socket %d | Crated in %s", Clients[i].socket, Clients[i].time);
        }
    }
}

void Status()
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, ip, INET_ADDRSTRLEN);
    
    if (server_fd) {
        printf("The server IP is %s \n", ip);
    }
}

void ps()
{
    printf("%s\n", PS);
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
        memset(buffer, 0, BUFF_SIZE);

        char* pcmd = Server_Command + 6;
        FILE* pf;
        pf = popen(pcmd, "r");

        while(fgets(buffer, sizeof(buffer), pf) != NULL) {
            send(client_fd, buffer, BUFF_SIZE, 0);
            memset(buffer, 0, BUFF_SIZE);
        }
        strcpy(buffer, "NULL");
        send(client_fd, buffer, BUFF_SIZE, 0);
        pclose(pf);
    }
}

char* IP;
void Server_CLI()
{
    char Server_Command[BUFF_SIZE];

    while(1) {

        printf(PROMPT);

        fgets(Server_Command, BUFF_SIZE, stdin);
        Server_Command[strcspn(Server_Command, "\n")] = '\0';

        if (strlen(Server_Command) <= 1)
            continue;

        if (strcmp(Server_Command, "up") == 0) {
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
            printf("up                         Start server\n"); 
            printf("down                       Stop server\n");
            printf("clients                    Show connection list\n");
            printf("status                     Show connection status\n");
            printf("ps                         Show process list\n");
            printf("history                    History\n");
        }
    }
}

void Add_Client(int new_client)
{
    for(int i = 0; i < CLIENTS_COUNT; i++) {

        if (Clients[i].socket == 0) {

            Clients[i].socket = new_client;
            time_t Ctime;
            time(&Ctime);
            strncpy(Clients[i].time, ctime(&Ctime), sizeof(Clients[i].time));
            break;
        }
    }
}
int main(int argc, char* argv[])
{

    Clients = mmap(NULL, sizeof(Tclients) * CLIENTS_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    memset(Clients, 0, sizeof(Tclients) * CLIENTS_COUNT);

    if (argc > 1) {
        IP = argv[1];
    } else {
        IP = "127.0.0.1";
    }

    pid_t pid = fork();

    if (pid == 0 )
    {
        Server_CLI();
        exit(0);
    }

    printf("\nSERVER CLI\n");

    /*Creating server socket*/

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: %s \n", strerror(errno));
    }

    /*Adding server config*/
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IP);
    address.sin_port = htons(PORT);

    /* Making server socket to listening mode*/
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printf("Error: %s \n", strerror(errno));
    }

    if (listen(server_fd, 3) < 0) {

        printf("Error: %s \n", strerror(errno));
    }

    while (1) {

        struct sockaddr_in client_address;
        socklen_t addrlen = sizeof(client_address);

        client_fd = accept(server_fd, (struct sockaddr*)&client_address, &addrlen);

        printf("-------------");
        printf("\nClient connected. Port is %u\n", ntohs(client_address.sin_port));
        printf("-------------");

        Add_Client(client_fd);
        pid_t new_pid = fork();

        if(new_pid == 0) {

            close(server_fd);

            while (waitpid(-1, NULL, WNOHANG)) {
                Client_Command();
            }

            exit(0);

        } else if (new_pid > 0) {
            continue;
        } else {
            printf("Error: %s \n", strerror(errno));
        }
    }
}
