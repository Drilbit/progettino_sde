#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>


int main(int argc, char* argv[])
{
    int sd;
    int type;
    struct sockaddr_in sa;
    in_addr_t address;
    char* addrserv = "127.0.0.1";
    int port_no = 10000;
    int reuse_opt = 1;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    inet_pton(AF_INET, addrserv, (void*)&address);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port_no);
    sa.sin_addr.s_addr = address;
    if (type == SOCK_STREAM)
        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse_opt, sizeof(int));

    bind(sd, (struct sockaddr*)&sa, sizeof(sa));
    printf("Socket %d associato a %s:%d\n", sd, addrserv, port_no);
    const int MAXQUEUE = 10;   
    int conn_sd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char addr_str[INET_ADDRSTRLEN];
    
    const int BUFFLEN = 1024;
    char buff[BUFFLEN];
    ssize_t rcvd_bytes, snd_bytes;
    
    pid_t pid;

    listen(sd, MAXQUEUE);
    while (1)
    {    
        printf("--- In attesa di connessione ---\n");
        conn_sd = accept(sd, (struct sockaddr*)&client_addr, &client_addr_len);
        if ((pid = fork()) < 0)
        {
            fprintf(stderr, "Impossibile creare un nuovo processo: %s\n", strerror(errno));
        }
        else if (pid == 0)
        {
           inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, addr_str, INET_ADDRSTRLEN);
            printf("Connesso col client %s:%d\n", addr_str, ntohs(client_addr.sin_port));
            while(1)
            {               
                rcvd_bytes = recv(conn_sd, &buff, BUFFLEN, 0);             
                buff[rcvd_bytes] = '\0';        
                printf("Ricevuto '%s'\n", buff);        
                snd_bytes = send(conn_sd, "OK", 2, 0);
                printf("Risposta inviata\n");
                if (strcmp(buff, "exit") == 0){
                    raise(SIGINT);
                    break;
                }
            }
            close(conn_sd);
            // termina il processo figlio
            exit(EXIT_SUCCESS);
        } 
    return EXIT_SUCCESS;
    }
}
