/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
/*MACROS*/
#define PORT 8080
#define BUFF_SIZE 2048
/*Global variables*/
char buff[1024];
int server_fd, status;
static int socket_id;
int connection[12];
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

    connection[socket_id] = accept(server_fd, (struct sockaddr*)&address,&addrlen);
    read(connection[socket_id] , buff, 1024);
    printf("%s\n",buff);
    send(connection[socket_id] , "SERVER TO CLIENT", 16, 0);
}

int Server_Down()
{
    for(int i = 0; i < 10 ; i++)
        close(connection[socket_id]);
    close(server_fd);
}

int *clients()
{
}

int ps()
{
    system("ps");
}

void history()
{

}
int main(int argc, char* argv[])
{
    printf("\nSERVER CLI\n");
    printf("USE help for help massage\n");
    /*Creating server socket*/
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket\n");
    }
    /*Adding server config*/
    if (setsockopt(server_fd, SOL_SOCKET,
                SO_REUSEADDR | SO_REUSEPORT, &opt,
                sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    /* Making server socket to listening mode*/
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {

        printf("bind\n");
    }
    int UP = 0;
    char buff[BUFF_SIZE];
    while(1) {
        if (UP)
            Server_Up();
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
        if (count > 1 && strcmp(str_command[1], "up") == 0) {
            UP = 1;
            Server_Up();
        } else if (count > 1 && strcmp(str_command[1], "down") == 0) {
            Server_Down();
        } else if (count > 1 && strcmp(str_command[1], "clients") == 0) {
            clients();
        } else if (count > 1 && strcmp(str_command[1], "ps") == 0) {
            ps();
        } else if (count > 1 && strcmp(str_command[1], "history") == 0) {
            history();
        } else {
            printf("Usage:\n");
            printf("COMMAND                     DESCRIPTION\n");
            printf("up:                         Start server\n");    
            printf("down:                       Stop server\n");    
            printf("client:                     Show connection list\n");    
            printf("ps:                         Show process list\n");    
            printf("history:                    History\n");    
        }
    }
}
