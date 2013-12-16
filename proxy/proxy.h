//Bibliothèques
	#include <stdio.h>
	#include <stdlib.h>
//Bibliothèques réseau
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <errno.h>
//Bibliothèques système
	#include <pthread.h>
	#include <semaphore.h>
//Bibliothèques autres
	#include <time.h>
	#include <string.h>
//Variables constantes
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket(s) close (s)
	#define PORT 2107
	#define TAILLE 1000000
	#define M 10//Nombre de limitation de connexion
//Redéfnition de types
	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr SOCKADDR;
//Structures
	typedef struct
	{
		SOCKET *socketClient;
		SOCKADDR_IN *sockAddrClient;		
	}structSocketClient;
//Variables déclarée globalement
	sem_t semaphoreSocket;
//-------------------------------------------Fonctions--------------------------------------//
	//Fonction recupHost.
	// Prend en paramètre une requete http et un lien.
	//Cette fonction renvoie le résultat directement dans lien.
	void recupHost(char *requete, char *lien)
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
	//Fonction nombreOctetRequete.
	// Prend en paramètre une requete http.
	//Cette fonction renvoie le nombre d'octet d'une requete.
	int nombreOctetRequete(char requete[])
	{
		printf("Requete web : \n %s\n",requete);
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
		}
		printf("--------------%d-------------\n",n);
		return n;
	}
//-------------------------------------------Fonctions thread---------------------------------
	//Fonction connexionClient qui prend en parametre une structure structSocketClient
	//La fonction gère la connexion
void *client(void *arg)
{
	//Variables
		char* requeteHTTPClient = calloc(1024,sizeof(char));
		char* reponseServeur = calloc(1024,sizeof(char));
		char* host = calloc(1024,sizeof(char));
	//Création de la nouvelle structure	
		structSocketClient *structSock = (structSocketClient *) arg;
	//Gestion du semaphore on la decremente
		sem_wait(&semaphoreSocket);
	//socket serveur web demande
		SOCKET *sockServeurWeb = malloc(sizeof(SOCKET));
		SOCKADDR_IN *sockAddrServeurWeb = malloc(sizeof(SOCKADDR_IN));
		socklen_t sockAddrServeurWebSize = sizeof(*sockAddrServeurWeb);
	//Création socket serveur web
		*sockServeurWeb = socket(AF_INET, SOCK_STREAM, 0);
	//Connexion d'un client
		printf("Un client se connecte avec la socket %d de %s:%d\n",*structSock->socketClient,inet_ntoa(structSock->sockAddrClient->sin_addr), htons(structSock->sockAddrClient->sin_port));
	//Reception requete HTTP du client
		recv(*structSock->socketClient,requeteHTTPClient,1024*sizeof(char),0);
	//Extraction de l'host dans la requete HTTP reçu
		recupHost(requeteHTTPClient,host);
	if(*sockServeurWeb != INVALID_SOCKET)
	{
		//Récuperation de l'adresse IP associé à l'host
			struct addrinfo *structAddrIP;
			if(getaddrinfo(host,NULL,NULL,&structAddrIP) != 0)
			{
				perror("Erreur getaddrinfo");
				exit(1);
			}
		//Configuration socket serveur web
			memcpy(sockAddrServeurWeb,structAddrIP->ai_addr,sockAddrServeurWebSize);
			sockAddrServeurWeb->sin_family = AF_INET;
			sockAddrServeurWeb->sin_port = htons(80);
			freeaddrinfo(structAddrIP);
		//Connexion au serveur web demandé
			printf("Connexion au serveur distant ..\n");
			connect(*sockServeurWeb,(SOCKADDR*)sockAddrServeurWeb,sockAddrServeurWebSize);
		//Envoie de la requete HTTP au serveur web
			printf("Envoie de la requete http ... \n");
			send(*sockServeurWeb,requeteHTTPClient,1024*sizeof(char),0);	
		//Réception de la requete HTTP et de la pageWeb du serveur web
			printf("Reception de la page web ... \n");			
			int length = 0;
			int recu = 0;
			while((recu = recv(*sockServeurWeb,reponseServeur + length,1024,0)) > 0)
			{
				length += recu;
				reponseServeur = realloc(reponseServeur,(1024 + length)*sizeof(char));
			}
			puts(reponseServeur);
		//Envoie de la requête HTTP au client
			printf("Envoie de la page web au client ...\n");
			send(*structSock->socketClient,reponseServeur,sizeof(reponseServeur),0);		
	}
	else
	{

	}	
	printf("Fin de traitement.\n");
	//Libération de la mémoire
		closesocket(*structSock->socketClient);
		closesocket(*sockServeurWeb);
		free(requeteHTTPClient);
		free(host);
		free(reponseServeur);
		free(sockServeurWeb);
		free(sockAddrServeurWeb);
		free(structSock->socketClient);
		free(structSock->sockAddrClient);
	//Libération semaphore
		sem_post(&semaphoreSocket);	
}
