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
//Bibliothèques cache
	#include "../Cache/cache.h"
//Bibliothèques Auth
	#include "../Auth/Auth.h"
//Bibliothèques Liste
	#include "../ListeGenerique/listeGenerique.h" 
//Bibliothèques autres
	#include <time.h>
	#include <string.h>
//Variables constantes
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket(s) close (s)
	#define TAILLE_PAGE_TELECHARGEMENT 100000
	#define TAILLE_PAGE_AUTH 100000
	#define PORT 65000
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
		Auth_Conf a_C;
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
	//Fonction nombreOctetRequete.
        // Prend en paramètre une requete http.
        //Cette fonction renvoie le nombre d'octet d'une requete.
	void enleveRequete(char requete[], char pageWeb[])
        {
                char *pointeur, *repere;
		int i=0;
                pointeur =  strstr(requete,"<!");
                if(pointeur != NULL)
                {
			while(*pointeur != '\0')
			{
				pageWeb[i] = *pointeur;
				pointeur++;
                		i++;
			}
                }
		printf("Page Web : %s\n",pageWeb);
        }
	//Fonction verifieTelechargement.
        // Prend en paramètre une requete http.
        //Cette fonction renvoie si c'est un téléchargement ou pas.
        int verifieTelechargement(char requete[])
        {
        	return 0;
        }
//-------------------------------------------Fonctions thread---------------------------------
	//Fonction connexionClient qui prend en parametre une structure structSocketClient
	//La fonction gère la connexion
void *client(void *arg)
{
	//Variables
		char* requeteHTTPClient = calloc(1024,sizeof(char));
		char* requeteHTTPServeur = calloc(1024,sizeof(char));
		char* pageWeb = calloc(1024,sizeof(char));
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
	//Vérification téléchargement
		if(verifieTelechargement(requeteHTTPClient) == 1)
		{
			printf("Telechargement detecte => REFUS\n");
			//Envoie de la page au client interdisant le téléchargement
				FILE *fTelechargement = NULL;
				fTelechargement = fopen("../pagesWeb/pageTelechargement.html","r");
				if(fTelechargement != NULL)
				{
					char* pageTelechargement = malloc(sizeof(char));
					int l = 0;
                                        char c;
                                        do
                                        {
                                         	c = fgetc(fTelechargement);
                                                pageTelechargement[l] = c;
                                                l++;
                                               	pageTelechargement = realloc(pageTelechargement,sizeof(char)*l);
                                        }
                                       	while(c != EOF);
					printf("Envoie page Web Telechargement\n");
					fclose(fTelechargement);
					send(*structSock->socketClient,pageTelechargement,sizeof(char)*sizeof(char)*l,0);
				}
			//Fin de traitement
				pthread_exit(NULL);
		}
	//Extraction de l'host dans la requete HTTP reçu
		recupHost(requeteHTTPClient,host);
	if(*sockServeurWeb != INVALID_SOCKET)
	{
		//Récuperation de l'adresse IP associé à l'host
			struct addrinfo *structAddrIP;
			if(getaddrinfo("graoully4.iutmetz.ad.univ-lorraine.fr",NULL,NULL,&structAddrIP) != 0)
			{
				perror("Erreur getaddrinfo");
				exit(1);
			}
		//Vérification de l'accesibilité de la page web
			int isAuth = isAuthorized(host,inet_ntoa(structSock->sockAddrClient->sin_addr));
        		if(isAuth <= 0)
			{
				printf("Lien non autorisé\n");
				//Envoie de la page au client interdisant le téléchargement
                                	FILE *fAuth = NULL;
                                	fAuth = fopen("../pagesWeb/pageAuth.html","r");
                                	if(fAuth != NULL)
                                	{
                                        	int l = 0;
						char* pageAuth = malloc(TAILLE_PAGE_AUTH*sizeof(char));
						char c;
                                        	do
                        			{
							c = fgetc(fAuth);
							pageAuth[l] = c;
							l++;
							printf("%c",c);
                        			}
						while(c != EOF);
						pageAuth[l-1] = '\0';
						free(pageAuth);
						printf("Envoie page Web Telechargement\n");
						fclose(fAuth);
                                       	 	send(*structSock->socketClient,pageAuth,sizeof(char)*l+1,0);
                                	}
                        	//Fin de traitement
                                	pthread_exit(NULL);
			}
                	printf("Lien autorisé\n");
		//Configuration socket serveur web
			memcpy(sockAddrServeurWeb,structAddrIP->ai_addr,sockAddrServeurWebSize);
			//sockAddrServeurWeb->sin_addr.s_addr=inet_addr("172.24.159.253");
			sockAddrServeurWeb->sin_family = AF_INET;
			sockAddrServeurWeb->sin_port = htons(3128);
			//freeaddrinfo(structAddrIP);
		//Connexion au serveur web demandé
			printf("Connexion au serveur distant ..\n");
			connect(*sockServeurWeb,(SOCKADDR*)sockAddrServeurWeb,sockAddrServeurWebSize);
		//Envoie de la requete HTTP au serveur web
			printf("Envoie de la requete http ... \n");
			send(*sockServeurWeb,requeteHTTPClient,1024*1024*sizeof(char),0);
		//Réception de la requete HTTP et de la pageWeb du serveur web
			printf("Reception de la page web ... \n");
			int length = 0;
			int recu = 0;
			FILE *f = NULL;
			f = fopen("pageTest.html","w+");
			while((recu = recv(*sockServeurWeb,requeteHTTPServeur + length,1024,0)) > 0)
			{
				length += recu;
				requeteHTTPServeur = realloc(requeteHTTPServeur,(1024 + length)*sizeof(char));
			}
			pageWeb = realloc(pageWeb,(1024 + length)*sizeof(char));
			enleveRequete(requeteHTTPServeur,pageWeb);
			fprintf(f,"%s\n",pageWeb);
			fclose(f);
		//Envoie de la requête HTTP au client
			printf("Envoie de la page web au client ...\n");
			send(*structSock->socketClient,pageWeb,sizeof(char)*length,0);
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
		free(pageWeb);
		free(sockServeurWeb);
		free(sockAddrServeurWeb);
		free(structSock->socketClient);
		free(structSock->sockAddrClient);
	//Libération semaphore
		sem_post(&semaphoreSocket);
	//Fin de traitement
		return NULL;
}