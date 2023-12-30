#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

char s_help[]= "help";
char s_add[] = "add";
char s_compress[]= "compress";
char s_quit[]= "quit";
char s_ok[] = "ok";

int add (char* percorso) {
	int len = strlen (percorso);
	int fd;
	int dim_file;
	struct stat st;
	//CONTROLLO dei caratteri validi
	for (int i = 0; i < len; i++) {
		if ( (percorso[i] < 'a' || percorso[i] > 'z') 
			&& (percorso[i] < 'A' || percorso[i] > 'Z')
			&& (percorso[i] != '.') ) {
			fprintf(stderr, "Errore! carattere '%s' non valido\n", percorso[i]);
			return -1;	//carattere non valido	
		}
	}
	
	//INVIO file al server
	if ( (fd = open(percorso, O_RDONLY)) == -1 ) {
		//errore
		fprintf(stderr, "Errore! %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if ( fstat(fd, &st) ) {
		//errore
		fprintf(stderr, "Errore! %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	dim_file = st.st_size;
	printf("Dimensione file %d\n", dim_file);
	printf("Invio del file %s al server in corso...\n", percorso);
	
	return 0;
}

int gui(char string[], char* arg){
	printf("string=%s arg=%s\n", string, arg);	//DEBUGGING
	if ( strcmp(string, s_ok) == 0){	//DEBUGGING
		printf("forse funziona\n");
		//wait(1);
		return 0;
	}
	else if(strcmp(string, s_help) == 0) {
		printf(" i comandi disponibili sono: \n");
		printf("%s: mostra elenco comandi\n", s_help);
		printf("%s: manda file al server\n", s_add);
		printf("%s: comprime archivio creato\n", s_compress);
		printf("%s: chiude il programma\n", s_quit);
		return 0;
	}
	else if (strcmp(string, s_quit) == 0) {
		raise(SIGINT);
	}
	else if (strcmp(string, s_add) == 0) {
		
		if (add(arg) == 0) {
			return 0;
		}
		return 1;
	}
	else if (strcmp(string, s_compress) == 0) {
		printf("funzione da implementare!\n");
		return 0;
	}
	else {
		printf("comando errato\n");
		//wait(1);
		return 1;
	}
}

int main(int argc, char* argv[]){
	const int DIM = 1024;
	char comando[DIM];
	char* arg;
	
	while(1){
		printf("comando?\n"); //se ci metto rcomp senza \n viene un effetto simile a quello richiesto.
		if (fgets(comando, DIM, stdin) == NULL) {
			return 1;
		}
		
		//SEPARA GLI ARGOMENTI	e TOGNLIE '\n' (Andrea)
		if ( (arg = strchr (comando, '\n')) != NULL ) {
			*arg = '\0';
		}
		if ( (arg = strchr (comando, ' ')) != NULL ) {
			*(arg++) = '\0';
		}
		//fine
		
		int status = gui(comando, arg);
		printf("status: %d\n", status); //DEBUGGING
	}
}
