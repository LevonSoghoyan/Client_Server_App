/*INCLUDES*/
#include <stdio.h>
#include <termios.h>
#include <sys/select.h>
#include <signal.h>
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
#define PORT 8088
#define BUFF_SIZE 20048
#define PROMPT "Client> "

/*Structs*/
struct termios origin_termios;

/*Global variables*/
char buf[BUFF_SIZE];
int client_fd = -1;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

void reset_terminal_mode() 
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin_termios);
}

void raw_mode() 
{
    if (!isatty(STDIN_FILENO)) return;
    if (tcgetattr(STDIN_FILENO, &origin_termios) < 0) {
        exit(EXIT_FAILURE);
    }

    atexit(reset_terminal_mode);

    struct termios raw = origin_termios;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    raw.c_oflag &= ~(OPOST);

    raw.c_cc[VEOF] = 4;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        exit(EXIT_FAILURE);
    }
}

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

void Deactivate();

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

    memset(buf, 0, BUFF_SIZE);
    recv(client_fd, buf, 11, 0);

    if (strcmp(buf,"SERVER DOWN") == 0) {
        Deactivate("SERVER DOWN");
    } else if (strcmp(buf,"SERVER FULL") == 0) {
        Deactivate("SERVER FULL");
    } else if (strcmp(buf,"SERVER STOP") == 0) {
        Deactivate("SERVER STOP");
    } else
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

void Deactivate(char* message)
{
    char* command = "disconnect";
    Shell(command);
    printf("%s\n", message);
    close(client_fd);
    client_fd = -1;
}
void D_connect()
{
    char* command = "disconnect";
    Shell(command);

    printf("Disconnected\n");
    close(client_fd);
    client_fd = -1;
}

void hendle(int sig)
{
    printf("SIGINT\n");
    char* command = "disconnect";
    Shell(command);
    send(client_fd,"SIGINT",6,0);
    close(client_fd);
    client_fd = -1;
}
int main(int argc, char* argv[])
{
    printf("\nCLIENT CLI\n");
    signal(SIGPIPE,SIG_IGN);
    signal(SIGINT,hendle);

    /*Adding server configurations*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    char buf[BUFF_SIZE];

    while(1) {

        char* input = readline(PROMPT);
        if (!input) break;
        strncpy(buf, input, BUFF_SIZE -1);   

        int No_Alpha = 0;
        while (No_Alpha < strlen(buf) && isalpha(buf[No_Alpha]) == 0) 
            No_Alpha++;


        for (int i = 0; i < BUFF_SIZE - No_Alpha; i++)
            buf[i] = buf[i + No_Alpha];

        if (strlen(buf) <= 1)
            continue;

        add_history(buf);

        char *token;
        token = strtok(buf, " ");
        token[strcspn(token,"\n")] = '\0';

        if (strcmp(token, "connect") == 0) {
            if (client_fd != -1)
                printf("Client already connected\n");
            else
                Connect((buf + strlen(token) + 1));
        } else if (!client_fd) {
            printf("Client disconnected\n");

        } else if (strcmp(token, "shell") == 0) {


            if (send(client_fd, buf, BUFF_SIZE, 0) < 0) {
                printf("Error: Connection lost.\n");
                close(client_fd);
                client_fd = -1;
                continue;
            }

            raw_mode();
            fd_set read_fds;
            int max_fd = (client_fd > STDIN_FILENO) ? client_fd : STDIN_FILENO;
            int server_dead = 0;

            while (1) {
                FD_ZERO(&read_fds);
                FD_SET(client_fd, &read_fds);
                FD_SET(STDIN_FILENO, &read_fds);

                if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
                    break;
                }
                if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
                    if (n <= 0) break;
                    if (write(client_fd, buf, n) < 0) {
                        server_dead = 1;
                        break;
                    }
                }

                if (FD_ISSET(client_fd, &read_fds)) {
                    ssize_t n = read(client_fd, buf, sizeof(buf));
                    if (n <= 0) {
                        server_dead = 1;
                        break;
                    }

                    if (n == 1 && buf[0] == 0x04) {
                        break; 
                    }

                    if (write(STDOUT_FILENO, buf, n) < 0) 
                        break;
                }
            }

            reset_terminal_mode(); 
            printf("\n");

            if (server_dead) {
                printf("Connection lost. Server shut down.\n");
                close(client_fd);
                client_fd = -1;
            }
        } else if (strcmp(token, "disconnect") == 0) {
            D_connect();
        } else if (strcmp(token, "status") == 0) {
            Status();
        } else if (strcmp(token, "exit") == 0) {
            break;
        } else {
            printf("Usage:\n");
            printf("COMMAND                     DESCRIPTION\n");
            printf("connect  <IP>               connect to server by <IP>\n");
            printf("shell <command>             run <command> in server terminal\n");
            printf("disconnect                  disconnect client \n");
            printf("status                      show connection status\n");
            printf("exit                        close terminal\n");
        }
    }
}
