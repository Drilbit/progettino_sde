#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

char help[]= "help";
char add[] = "add";
char compress[]= "compress";
char quit[]= "quit";
char ok[] = "ok";
char connetti[] = "connetti";

int gui(char string[], int socketdescriptor){
	if ( strcmp(string, ok) == 0){
		printf("forse funziona\n");
		wait(1);
		return 0;
	} else if(strcmp(string, help) == 0) {
		printf(" i comandi disponibili sono: \n");
		printf("%s: mostra elenco comandi\n", help);
		printf("%s: manda file al server\n", add);
		printf("%s: comprime archivio creato\n", compress);
		printf("%s: chiude il programma\n", quit);
		printf("%s: connette col server \n", connetti);
		return 0;
	} else if (strcmp(string, quit) == 0) {
		raise(SIGINT);
	}else if (strcmp(string, add) == 0 || strcmp(string, compress) == 0) {
		printf("funzione da implementare!\n");
		return 0;
	} else if (strcmp(string, connetti) == 0){
			client(socketdescriptor);
	} else {
		printf("comando errato\n");
		wait(1);
		return 1;
	}
}

int client(int sod)
{
	const int MAXLEN = 1024;
	char buff[MAXLEN];
	int buff_len;
	char resp[3];
	ssize_t snd_bytes, rcvd_bytes;
    while(1)
    {
        fgets(buff, MAXLEN, stdin);
        buff_len = strlen(buff);
        if (buff[buff_len - 1] == '\n')
            buff[buff_len - 1] = '\0';
        snd_bytes = send(sod, buff, strlen(buff), 0);
        printf("Inviati %d bytes\n", snd_bytes);
	rcvd_bytes = recv(sod, &resp, 2, 0);
        resp[rcvd_bytes] = '\0';
        printf("Ricevuta risposta '%s'\n", resp);
        if (strcmp(buff, "exit") == 0)
            break;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
	const int DIM = 1024; //comando
	char comando[DIM]; //comando 
	int sd;
	int type;
	struct sockaddr_in sa;
	in_addr_t address;
	char* addr_str = "127.0.0.1";
	int port_no = 10000;
	sd = socket(AF_INET, SOCK_STREAM, 0);
	inet_pton(AF_INET, addr_str, (void*)&address);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port_no);
	sa.sin_addr.s_addr = address;
	printf("Connessione...\n");
	if((connect(sd, (struct sockaddr*)&sa, sizeof(struct sockaddr_in))) >= 0 ){
		printf("connesso, pronto\n");
	while(1){
		printf("comando?\n"); //se ci metto rcomp senza \n viene un effetto simile a quello richiesto.
		scanf("%s", comando);
		int status = gui(comando, sd);
		printf("status: %d\n", status); 
		}
	}
}

