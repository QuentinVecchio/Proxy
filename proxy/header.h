//Bibliothèques à inclure
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
//VARIABLES CONSTANTES
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close (s)
#define PORT 2107
#define TAILLE_LIEN 1000
#define TAILLE_NOM 256
#define TAILLE_ENVOIE 1000
//AUTRES

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

//Fonctions
char host[10000];
char lien[1000];
char pageWeb[1000000];
void recupHost(char requete[], char lien[])
{
	int x =0;
	while(requete[x+11] != '/')
	{
		lien[x] = requete[x+11];
		x++;
	}
	printf("%s\n",lien);
}
