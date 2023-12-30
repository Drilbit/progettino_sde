#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



int main(int argc, char* argv[]){
	int sd;
	struct sockaddr_in sa;
	in_addr_t address;
	char* addr_str = "127.0.0.1";
	int port_no = 10000;
	int reuse_opt = 1;
	int statussocket; //debug
	statussocket = socket(AF_INET, SOCK_STREAM, 0);
	sd = statussocket; //debug
	printf("status socket: %d\n", statussocket); //debug
	int statusinet; //debug
	statusinet = inet_pton(AF_INET, addr_str, (void*)&address);
	printf("status inet: %d\n", statusinet); //debug
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port_no);
	sa.sin_addr.s_addr = address;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse_opt, sizeof(int));
	int statusbind; //debug
	statusbind = bind(sd, (struct sockaddr*)&sa, sizeof(sa));
	printf("status bind: %d\n", statusbind); //debug
	
	//parte funzione nardini
	
	const int MAXQUEUE = 10;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	// char addr_str[INET_ADDRSTRLEN]; debug, se lasciato fa conflitto con il char* sopra e non capisco perché.
	const int BUFFLEN = 4096;
	char buff[BUFFLEN];
	ssize_t rcvdbytes, sndbytes;
	pid_t pid;
	int statuslisten; //debug
	statuslisten = listen(sd, MAXQUEUE);
	printf("status listen: %d\n", statuslisten); //debug
	while(1){
		sd = accept(sd, (struct sockaddr*)&client_addr, &client_addr_len);
		printf("status connessione: %d\n", sd); //debug
		pid = fork();
		printf("status pid: :%d\n", pid); //debug
		if (pid < 0){
			printf("errore");
		} else if (pid == 0){
			if(inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, addr_str, INET_ADDRSTRLEN) < 0 ){
				printf("errore nella seconda inetntop");
			}
			printf("connesso con %s:%d\n", addr_str, ntohs(client_addr.sin_port));
			printf("sonoqui"); //debug
			while(1){
				rcvdbytes = recv(sd, &buff, BUFFLEN, 0); //if di controllo se riesce a ricevere
				buff[rcvdbytes] = '\0';
				printf("ricevuto %s\n", buff);
				sndbytes = send(sd, "OK", 2, 0); //if controllo se riesce a mandare
				if (strcmp(buff, "exit") == 0){
					break;
				}
			}
		}		
	}
}
