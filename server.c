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
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/*MACROS*/
#define PORT 8080
#define BUFF_SIZE 2048
#define CLIENTS_COUNT 10
#define HISTORY_SIZE  100
#define PROMPT "Server> "

/*Global variables*/
char Shell_Command[BUFF_SIZE];
char PS[BUFF_SIZE];
char History[HISTORY_SIZE][BUFF_SIZE];
struct sockaddr_in address;
socklen_t add_size = sizeof(address);
int opt = 1;
int client_fd;
char* IP;

/*Struct*/
typedef struct Clients {
    int socket;
    char ip[INET_ADDRSTRLEN];
    int port;
    int pid;
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
    printf("ID       SOCKET     PID             CREATED-ON\n");
    int id = 0;
    for(int i = 0; i < CLIENTS_COUNT; i++) {
        id++;
        if (Clients[i].socket != 0) {
            printf("%d        %d          %d         %s", id, Clients[i].socket, Clients[i].pid, Clients[i].time);
        }
    }
}

void Status()
{
    printf("ID       PORT      IP\n");
    int id = 0;
    for(int i = 0; i < CLIENTS_COUNT; i++) {
        id++;
        if (Clients[i].socket != 0) {
            printf("%d        %d          %s\n", id, Clients[i].port, Clients[i].ip);
        }
    }
}

void ps()
{
    printf("%s\n", PS);
}

void history()
{

}

void Remove_Client(int new_client)
{

    for(int i = 0; i < CLIENTS_COUNT; i++) {

        if (Clients[i].socket == new_client) {
            Clients[i].socket = 0;
            break;
        }
    }
}

void Client_Command()
{
    memset(Shell_Command, 0, BUFF_SIZE);
    int bytes = recv(client_fd , Shell_Command, BUFF_SIZE - 1, 0);
    Shell_Command[bytes] = '\0';
    Shell_Command[strcspn(Shell_Command, "\r\n")] = '\0';

    if (strncmp(Shell_Command, "shell", 5) == 0) {
        char Formated_Command[BUFF_SIZE * 2];
        char buffer[BUFF_SIZE];

        memset(buffer, 0, BUFF_SIZE);
        
        char* pcmd = Shell_Command + 6;
        pcmd[strcspn(pcmd, "\r\n")] = '\0';

        snprintf(Formated_Command, sizeof(Formated_Command), "%s 2>&1", pcmd);
        FILE* pf;
        pf = popen(Formated_Command, "r");       
        while(fgets(buffer, sizeof(buffer), pf) != NULL) {
            send(client_fd, buffer, strlen(buffer), 0);
            memset(buffer, 0, BUFF_SIZE);
        }
        strcpy(buffer, "NULL");
        send(client_fd, buffer, strlen(buffer), 0);
        pclose(pf);
    } else if (strcmp(Shell_Command, "disconnect") == 0) {
        Remove_Client(client_fd);
        close(client_fd);
        exit(0);
    }
}

void Server_CLI()
{
    char Server_Command[BUFF_SIZE];

    while(1) {
        char * input = readline(PROMPT);
        strncpy(Server_Command, input, BUFF_SIZE - 1);
        add_history(Server_Command);
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
    struct sockaddr_in add;
    int res = getpeername(new_client, (struct sockaddr *)&add, &add_size);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &add.sin_addr, ip, INET_ADDRSTRLEN);
    for(int i = 0; i < CLIENTS_COUNT; i++) {

        if (Clients[i].socket == 0) {

            Clients[i].socket = new_client;
            time_t Ctime;
            time(&Ctime);
            strncpy(Clients[i].time, ctime(&Ctime), sizeof(Clients[i].time));
            Clients[i].port = ntohs(add.sin_port);
            Clients[i].pid = getpid();
            strcpy(Clients[i].ip, ip);
            break;
        }
    }
    printf("\nClient connected. Port: %u  IP: %s\n", ntohs(add.sin_port), ip);
}

int main(int argc, char* argv[])
{

    Clients = mmap(NULL, sizeof(Tclients) * CLIENTS_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    memset(Clients, 0, sizeof(Tclients) * CLIENTS_COUNT);

    if (argc > 1) {
        IP = argv[1];
        while (!inet_pton(AF_INET, IP, &(address.sin_addr))) {
            printf("Invalid IP address try again \n");
            IP = (char*) malloc(30 * sizeof(char));
            scanf("%28s", IP);
        }
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

    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: %s \n", strerror(errno));
        return 0;
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

        client_fd = accept(server_fd, (struct sockaddr*)&client_address, &add_size);

        Add_Client(client_fd);
        pid_t new_pid = fork();

        if(new_pid == 0) {

            close(server_fd);

            while (1) {
                Client_Command();
            }
            /*
            while (waitpid(-1, NULL, WNOHANG)) {
                Client_Command();
            }*/

            exit(0);

        } else if (new_pid > 0) {
            continue;
        } else {
            printf("Error: %s \n", strerror(errno));
        }
    }
}
