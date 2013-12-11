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
#include <pthread.h>
//VARIABLES CONSTANTES
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close (s)
#define PORT 8000
#define TAILLE 1000000
#define NB_MAX 10
//AUTRES

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

//Structures
	typedef struct
	{
		SOCKET csock;		
	}structSocketClient;
//Fonctions
void recupHost(char requete[], char lien[])
{
	printf("Requete : \n%s\n",requete);
	int x = 0;
	int z = 0;
	while(requete[x] != ':')
	{
		x++;
	}
	x = x+3;
	while(requete[x] != '/')
	{
		lien[z] = requete[x];
		x++;
		z++;
	}
	printf("Host demandé : %s\n",lien);
}

void *client(void *arg)
{
	structSocketClient *sClient = (structSocketClient*) arg;
	//socket serveur demande
		SOCKADDR_IN serveurSin;
		SOCKET serveurSock;
		SOCKADDR_IN csin;
	//Variables
		char host[TAILLE];
		char lien[TAILLE];
		char pageWeb[TAILLE];
	//printf("Un client se connecte avec la socket %d de %s:%d\n",sClient->csock,inet_ntoa(csin.sin_addr), htons(csin.sin_port));
	recv(sClient->csock,host,sizeof(host),0);
	recupHost(host,lien);
	//Configuration socket serveur demandé
	serveurSock = socket(AF_INET, SOCK_STREAM, 0);
	if(serveurSock != INVALID_SOCKET)
	{
		//Configuration
		struct hostent *monHost;
		monHost = gethostbyname(lien);
		memcpy((char *)&serveurSin.sin_addr,(char *)monHost->h_addr,monHost->h_length);
		serveurSin.sin_family = AF_INET;
		serveurSin.sin_port = htons(80);
		//printf("Adresse IP du serveur demandé : %s\n",inet_ntoa(serveurSin.sin_addr));
		connect(serveurSock,(SOCKADDR*)&serveurSin, sizeof(serveurSin));
		//printf("Connexion au serveur distant ..\n");
		send(serveurSock,host,sizeof(host),0);
		//printf("Envoie de la requete http ... \n");
		recv(serveurSock,pageWeb,sizeof(pageWeb),0);
		//printf("Page web reçu ...\n");
		//printf("Fermeture de la socket Serveur Web.\n");
		closesocket(serveurSock);
		//printf("Envoie de la page web au client ...\n");
		send(sClient->csock,pageWeb,sizeof(pageWeb),0);
		//printf("ok\n");
	}
	else
		perror("connexion serveur demande");
}
