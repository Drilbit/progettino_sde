#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

const int MAXLEN = 1024;

int compress(){
	//execlp("tar", "tar", "-c", "-v", "-z","-f", "nome archivio", buffer(cartella dove prendi i file), (char*) 0 );
	//alternativa
	//system("tar"); 
	
	//la creazione della cartella va nel main a priori perché 3 righe. con mkdir e permessi.
}

int add (char* percorso, int sk) {
	int len = strlen (percorso);	//lunghezza nome file
	int fd;			//file descriptor
	int dim_file;	//dimensione del file
	int rd;	//byte letti
	struct stat st;	//struttura per lettura dati file
	char buff[MAXLEN];	//buffer di appoggio

	//CONTROLLO dei caratteri validi
	for (int i = 0; i < len; i++) {
		if ( (percorso[i] < 'a' || percorso[i] > 'z') 
			&& (percorso[i] < 'A' || percorso[i] > 'Z')
			&& (percorso[i] != '.') ) {
			fprintf(stderr, "Errore! carattere '%s' non valido\n", percorso[i]);
			return -1;	//carattere non valido	
		}
	}
	
	//APERTURA FILE
	if ( (fd = open(percorso, O_RDONLY)) == -1 ) {
		//errore
		fprintf(stderr, "Errore! File inesistente\n");
		return -1;
	}

	//LETTURA DIMENSIONE
	if ( fstat(fd, &st) ) {
		//errore
		fprintf(stderr, "Errore! %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	dim_file = st.st_size;
	printf("Dimensione file %d\n", dim_file);

	//INVIO FILE AL SERVER
	printf("Invio del file %s al server in corso...\n", percorso);
	send(sk, percorso, len, 0);		//invia nome del file
	while (read(fd, buff, MAXLEN)) {//invia file	
		send(sk, buff, MAXLEN, 0);
	}
	printf("File inviato con successo\n");

	return 0;
}

int controlc(int segnale){
	//SE segnale segnale = SIGINT è come mandare comando quit. chiudo con una exit.
}

int main(int argc, char* argv[])
{
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
	
	char buff[MAXLEN];
	int buff_len;
	char resp[3];
	ssize_t snd_bytes, rcvd_bytes;
	char* arg;	//Argomento del comando letto da tastiera

	printf("Connessione...\n");
	if( ( connect(sd, (struct sockaddr*)&sa, sizeof(struct sockaddr_in)) ) >= 0 ) {
		printf("connesso, pronto\n");
	}
    while(1)
    {	
		//LETTO COMANDO DA TASTIERA
        fgets(buff, MAXLEN, stdin);
        buff_len = strlen(buff);
        
		//SEPARA GLI ARGOMENTI	e TOGLIE  EVENTUALE '\n' A FONDO STRINGA
		if ( (arg = strchr (buff, '\n')) != NULL ) {
			*arg = '\0';	//rimuove '\n'	(uso arg come appoggio)
		}
		if ( (arg = strchr (buff, ' ')) != NULL ) {
			*(arg++) = '\0';	//mette terminatore a metà per separare comando e argomento
		}

		//INVIO COMANDO A SERVER
        snd_bytes = send(sd, buff, strlen(buff), 0);
        printf("Inviati %d bytes\n", snd_bytes);

		//RISPOSTA DA SERVER
		rcvd_bytes = recv(sd, &resp, 2, 0);
        resp[rcvd_bytes] = '\0';
        printf("Ricevuta risposta '%s'\n", resp);

		//ESECUZIONE COMANDO
        if (strcmp(buff, "exit") == 0)
            break;
		else if (strcmp(buff, "add") == 0)
			add(arg, sd);
    }
    
    return EXIT_SUCCESS;
}