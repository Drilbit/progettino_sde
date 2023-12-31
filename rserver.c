#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>

const char nome_cartella[] = "fileserver/";
const char cartella[] = "mkdir -m 777 fileserver";

int connessione(int sd){

}

int add (int sk) {
    const int MAX_LEN = 1024;
	char buff[MAX_LEN];
    int fd; //file descriptor
    mode_t permessi = (S_IRUSR || S_IWUSR || S_IXUSR
                    || S_IRGRP || S_IWGRP || S_IXGRP
                    || S_IROTH || S_IWOTH || S_IXOTH);
    
    int rcv;    //byte ricevuti
    
    //CREAZIONE FILE
    strcpy(buff, nome_cartella);
    printf("directory: %s\n", buff);
    rcv = recv(sk, &( buff[strlen(nome_cartella)] ), MAX_LEN, 0);
    buff[strlen(nome_cartella) + rcv] = '\0';
    printf("nuovo file: %s\n", buff);
    fd = open(buff, O_CREAT || O_RDONLY, permessi);
    printf("file descriptor: %d\n", fd);
    //se già file con stesso nome??

    //COPIA CONTENUTI FILE
    while ( (rcv = recv(sk, &buff, MAX_LEN, 0)) == MAX_LEN) {
        write(fd, &buff, rcv);
    }

	return 0;
}

int main(int argc, char* argv[])
{
	int creazionecartella = system(cartella);
	if(creazionecartella == 0){
		printf("cartella creata correttamente\n");
	} else {
		printf("errore creazione cartella \n");
	}
	
    int socketdescriptor;
    int type;
    struct sockaddr_in server_address;
    in_addr_t address;
    char* indirizzo_server = "127.0.0.1";
    int porta_numero = 10000;
    int reuse_opt = 1;
    socketdescriptor = socket(AF_INET, SOCK_STREAM, 0);
    inet_pton(AF_INET, indirizzo_server, (void*)&address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(porta_numero);
    server_address.sin_addr.s_addr = address;
    if (type == SOCK_STREAM)
        setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, &reuse_opt, sizeof(int));

    bind(socketdescriptor, (struct sockaddr*)&server_address, sizeof(server_address));
    printf("Socket %d associato a %s:%d\n", socketdescriptor, indirizzo_server, porta_numero);
    const int MAXQUEUE = 10;   
    int conn_sd;
    struct sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(client_address);
    char addr_str[INET_ADDRSTRLEN];
    
    const int BUFFLEN = 4096;
    char buff[BUFFLEN];
    ssize_t byte_ricevuti, byte_mandare;
    
    pid_t pid;

    listen(socketdescriptor, MAXQUEUE);
    while (1)
    {    
        printf("--- In attesa di connessione ---\n");
        conn_sd = accept(socketdescriptor, (struct sockaddr*)&client_address, &client_addr_len);
        if ((pid = fork()) < 0)
        {
            fprintf(stderr, "Impossibile creare un nuovo processo: %s\n", strerror(errno));
        }
        else if (pid == 0)
        {
            inet_ntop(AF_INET, &client_address.sin_addr.s_addr, addr_str, INET_ADDRSTRLEN);
            printf("Connesso col client %s:%d\n", addr_str, ntohs(client_address.sin_port));
            while(1)
            {               
                byte_ricevuti = recv(conn_sd, &buff, BUFFLEN, 0);             
                buff[byte_ricevuti] = '\0';        
                printf("Ricevuto '%s'\n", buff);        
                byte_mandare = send(conn_sd, "OK", 2, 0);
                printf("Risposta inviata\n");
                if (strcmp(buff, "exit") == 0){
                    raise(SIGINT);
                    break;
                }
                else if (strcmp(buff, "add") == 0)
			        add(conn_sd);
            }
            close(conn_sd);
            // termina il processo figlio
            exit(EXIT_SUCCESS);
        }
    }
}