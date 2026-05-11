/*INCLUDES*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
/*MACROS*/
#define PORT 8080
char buff[1024];
int server_fd, status;
static int socket_id;
int connection[12];
/********************************
 *@breef


 ******************************/
int Server_Up()
{
    /*Creating server socket*/

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int opt = 1;
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

    if (listen(server_fd, 3) < 0) {

        printf("listen\n");
    }

    if (connection[socket_id] 
            = accept(server_fd, (struct sockaddr*)&address,&addrlen)) {

        printf("accept\n");
    }

    read(connection[socket_id] , buff, 1024);
    printf("%s\n",buff);
    send(connection[socket_id] , "SERVER TO CLIENT", 16, 0);
    close(connection[socket_id] );
    close(server_fd);
}

int Server_Down()
{
}

int *clients()
{

}

int ps()
{

}

void history()
{

}
int main(int argc, char* argv[])
{
    Server_Up();
}
