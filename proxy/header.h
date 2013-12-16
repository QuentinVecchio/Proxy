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
#include <semaphore.h>
//VARIABLES CONSTANTES
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close (s)
#define PORT 2107
#define TAILLE 1000000
#define NB_MAX 10
#define M 10 //Nombre de limitation du client
//AUTRES

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;


//VARIABLE GLOBALE

sem_t semaphoreSocket;

//Structure utilisé pour instencier un thread
typedef struct
{
	SOCKET *csock;
	SOCKADDR_IN *csin;		
}structSocketClient;
//Fonctions
void recupHost(char requete[], char lien[])
{
	printf("Requete : \n%s\n",requete);
	char *pointeur;
	int i=0;	
	pointeur =  strstr(requete,"Host: ");
	if(pointeur != NULL)
	{
		pointeur += 6;
		while(*pointeur != '\n')
		{
			lien[i] = *pointeur;
			i++;
			pointeur++;
		}
		lien[i-1] = '\0';
	}
	printf("Host demandé : %s\n",lien);
}
int nombreOctetRequete(char requete[])
{
	//printf("Requete web : \n %s\n",requete);
	char *pointeur;
	char nombre[100];
	int n = -1, i=0;	
	pointeur =  strstr(requete,"Content-Length: ");
	if(pointeur != NULL)
	{
		n = 0;
		pointeur += 16;
		while(*pointeur != '\n')
		{
			nombre[i] = *pointeur;
			i++;
			pointeur++;
		}
		n = atoi(nombre);
		n += 1024;
	}
	return n;
}
//Fonctions thread qui prend en paramètre la socket cliente
void *client(void *arg)
{
	//Instanciation de la structure contenant la socket cliente
		structSocketClient *sClient = (structSocketClient*) arg;
	//Gestion du semaphore on la decremente
		sem_wait(&semaphoreSocket);
	//socket serveur demande
		SOCKADDR_IN serveurSin;
		SOCKET serveurSock;
	//Variables
		char requeteClient[TAILLE];
		char lien[TAILLE];
	//Connexion d'un client et reception de la requete HTTP
		printf("Un client se connecte avec la socket %d de %s:%d\n",*sClient->csock,inet_ntoa(sClient->csin->sin_addr), htons(sClient->csin->sin_port));
		recv(*sClient->csock,requeteClient,sizeof(requeteClient),0);
	//Récuperation de l'host dans la requete	
		recupHost(requeteClient,lien);
	//Configuration socket serveur demandé
	serveurSock = socket(AF_INET, SOCK_STREAM, 0);
	if(serveurSock != INVALID_SOCKET)
	{
		//Configuration de la socket qui va aller chercher la page web
			struct addrinfo *res = NULL;
			getaddrinfo(lien,NULL,NULL,&res);
			serveurSin = *((SOCKADDR_IN *) res->ai_addr);
			serveurSin.sin_family = AF_INET;
			serveurSin.sin_port = htons(80);			
			/*struct hostent *monHost;
			monHost = gethostbyname(lien);*/	
			printf("Adresse IP du serveur demandé : %s\n",inet_ntoa(serveurSin.sin_addr));
			connect(serveurSock,(SOCKADDR*)&serveurSin, sizeof(serveurSin));
		//Connexion au serveur web demandé et reception de l'en tete HTTP
			printf("Connexion au serveur distant ..\n");
			send(serveurSock,requeteClient,sizeof(requeteClient),0);
			printf("Envoie de la requete http ... \n");
			char enTeteServeurRecu[TAILLE];
			recv(serveurSock,enTeteServeurRecu,sizeof(enTeteServeurRecu),0);
			send(*sClient->csock,enTeteServeurRecu,sizeof(enTeteServeurRecu),0);
			//int nOctet = nombreOctetRequete(enTeteServeurRecu);
			//char *pageWeb = NULL;
			//pageWeb = (char *) malloc(nOctet * sizeof(char));
			//recv(serveurSock,pageWeb,sizeof(pageWeb),0);
			//printf("%s\n",pageWeb);	
			printf("Page web reçu ...\n");
		//Fermeture de la socket serveur web		
			printf("Fermeture de la socket Serveur Web.\n");
			closesocket(serveurSock);
		//Envoie de la page web au vlient		
			printf("Envoie de la page web au client ...\n");
			//send(*sClient->csock,pageWeb,sizeof(pageWeb),0);
		//Fermeture de la socket cliente
			printf("Fermeture de la socket cliente.\n");
			closesocket(*sClient->csock);
	}
	else
		perror("connexion serveur demande");
	//Libération semaphore
		sem_post(&semaphoreSocket);
}
