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
	#define KB_SIZE 1024
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
		Cache_Elt* e;
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
	//Fonction recupTitrePage.
        // Prend en paramètre une requete http et le nom.
        //Cette fonction renvoie le nom de la page web voulu.
	void recupTitrePage(char requete[], char nom[])
        {
                char *pointeur, *repere;
		int i=0;
                pointeur =  strstr(requete,"GET ");
		repere = strstr(requete,"HTTP/1.1");
                if(pointeur != NULL)
                {
			pointeur+= 4;
			if(repere != NULL)
			{
				while(pointeur != repere)
				{
					nom[i] = *pointeur;
					pointeur++;
                			i++;
				}
                	}
		}
		printf("Nom : %s\n",nom);
        }
	//Fonction verifieTelechargement.
        // Prend en paramètre une requete http.
        //Cette fonction renvoie si c'est un téléchargement ou pas.
        int verifieTelechargement(char requete[])
        {
        	return 0;
        }
	//Fonction contenuFichier.
        // Prend en paramètre un lien vers un fichier et la taille du contenu.
        //Cette fonction renvoie le contenur d'un fichier.
        char *contenuFichier(char *lienFichier,int* l)
        {
		char *contenu = malloc(sizeof(char));
		FILE *file = NULL;
               	file = fopen(lienFichier,"r");
                if(file != NULL)
                {
                     	int t = 0;
                        char c;
                        do
                        {
                         	c = fgetc(file);
                                contenu[t] = c;
				t += 1;
                                contenu = realloc(contenu,sizeof(char)*t+1);
                        }
                        while(c != EOF);
                        contenu[t-2];
                        fclose(file);
			*l = t;
               	}
		return contenu;
        }
//-------------------------------------------Fonctions thread---------------------------------
	//Fonction connexionClient qui prend en parametre une structure structSocketClient
	//La fonction gère la connexion
void *client(void *arg)
{
	//Variables
		char requeteHTTPClient[KB_SIZE*30+1];
		char reponseServeur[KB_SIZE*4+1];
		char host[1024];
		char nomPage[1024];
		int length = 0, recu = 0;
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
		recv(*structSock->socketClient,requeteHTTPClient,sizeof(requeteHTTPClient),0);
	//Vérification téléchargement
		if(verifieTelechargement(requeteHTTPClient) == 1)
		{
			printf("Telechargement detecte => REFUS\n");
			//Envoie de la page au client interdisant le téléchargement
				int l;
				char* pageTelechargement = contenuFichier("../pagesWeb/pageTelechargement.html",&l);
				if(l > 0)
				{
					printf("Envoie de la page de téléchargement.\n");
					printf("%s\n",pageTelechargement);
					send(*structSock->socketClient,pageTelechargement,sizeof(char)*(l-2),0);
					free(pageTelechargement);
				}
				//Libération de la mémoire
                                        closesocket(*structSock->socketClient);
                                        closesocket(*sockServeurWeb);
                                        free(sockServeurWeb);
                               		free(sockAddrServeurWeb);
                                      	free(structSock->socketClient);
                                      	free(structSock->sockAddrClient);
                              	//Libération semaphore
                                      	sem_post(&semaphoreSocket);
                               	//Fin de traitement
                             	      	return NULL;
		}
	//Extraction de l'host dans la requete HTTP reçu
		recupHost(requeteHTTPClient,host);
	//Extraction du nom de la page web demandée
		recupTitrePage(requeteHTTPClient,nomPage);
	if(*sockServeurWeb != INVALID_SOCKET)
	{
		/*//Récuperation de l'adresse IP associé à l'host
			struct addrinfo *structAddrIP;
			if(getaddrinfo("graoully4.iutmetz.ad.univ-lorraine.fr",NULL,NULL,&structAddrIP) != 0)
			{
				perror("Erreur getaddrinfo");
				exit(1);
			}*/
			struct addrinfo *structAddrIP;
                        if(getaddrinfo(host,NULL,NULL,&structAddrIP) != 0)
                        {
                                printf("Erreur 404\n");
                                //Envoie de la page au client interdisant cette page
                                        int l;
                                        char* pageErr = contenuFichier("../pagesWeb/page404.html",&l);
                                        if(l > 0)
                                        {
                                                printf("Envoie de la page 404.\n");
                                                send(*structSock->socketClient,pageErr,sizeof(char)*(l-2),0);
                                                free(pageErr);
                                        }
                                        //Libération de la mémoire
                                                closesocket(*structSock->socketClient);
                                                closesocket(*sockServeurWeb);
                                                free(sockServeurWeb);
                                                free(sockAddrServeurWeb);
                                                free(structSock->socketClient);
                                                free(structSock->sockAddrClient);
                                        //Libération semaphore
                                                sem_post(&semaphoreSocket);
                                        //Fin de traitement
                                                return NULL;
                        }
		//Vérification de l'accesibilité de la page web
			int isAuth = isAuthorized(host,inet_ntoa(structSock->sockAddrClient->sin_addr));
        		if(isAuth <= 0)
			{
				printf("Lien non autorisé\n");
				//Envoie de la page au client interdisant cette page
                                	int l;
                                	char* pageAuth = contenuFichier("../pagesWeb/pageAuth.html",&l);
                                	if(l > 0)
                                	{
                                        	printf("Envoie de la page d'authorisation.\n");
                                        	send(*structSock->socketClient,pageAuth,sizeof(char)*(l-2),0);
                                        	free(pageAuth);
                                	}
                                	//Libération de la mémoire
                				closesocket(*structSock->socketClient);
                				closesocket(*sockServeurWeb);
                				free(sockServeurWeb);
                				free(sockAddrServeurWeb);
                				free(structSock->socketClient);
                				free(structSock->sockAddrClient);
        				//Libération semaphore
                				sem_post(&semaphoreSocket);
        				//Fin de traitement
                				return NULL;
			}
                	printf("Lien autorisé\n");
		//Recherche dans le cache
			Cache_Elt* elt = malloc(sizeof(Cache_Elt));
			while(1)
			{
				//Recherche de la page dans le cache
					elt = isInCache(nomPage);
				//Si la page y est alors on la lit
				if(elt != NULL)
				{
					printf("Page web trouvé dans le cache.\n");
                        		//Envoie de la page au client
                        	        int l;
                        	        char* pageCache = contenuFichier(elt->path,&l);
                        	        if(l > 0)
                        	        {
                                	        printf("Envoie de la page.\n");
                                	        send(*structSock->socketClient,pageCache,sizeof(char)*(l-2),0);
                                	        free(pageCache);
                                	}
					//Envoie de la page web au client
                                		printf("Envoie de la page web au client ...\n");
						printf("PAGE WEB DANS LE CACHE : \n%s\n",pageCache);
                                		send(*structSock->socketClient,pageCache,sizeof(char)*(l-2),0);
					//On quitte la boucle
					break;
				}
				//Sinon on la télécharge
				else
				{
					//Configuration socket serveur web
						memcpy(sockAddrServeurWeb,structAddrIP->ai_addr,sockAddrServeurWebSize);
						//sockAddrServeurWeb->sin_addr.s_addr=inet_addr("172.24.159.253");
						sockAddrServeurWeb->sin_family = AF_INET;
						//sockAddrServeurWeb->sin_port = htons(3128);
						sockAddrServeurWeb->sin_port = htons(80);
							printf("%s\n",inet_ntoa(sockAddrServeurWeb->sin_addr));
						freeaddrinfo(structAddrIP);
					//Connexion au serveur web demandé
						printf("Connexion au serveur distant ..\n");
						connect(*sockServeurWeb,(SOCKADDR*)sockAddrServeurWeb,sockAddrServeurWebSize);
					//Envoie de la requete HTTP au serveur web
						printf("Envoie de la requete http ... \n");
						send(*sockServeurWeb,requeteHTTPClient,sizeof(requeteHTTPClient),0);
					//Configuration de la nouvelle entrée dans le cache
						Cache_Elt* element = generate(nomPage);
					//Réception de la requete HTTP et de la pageWeb du serveur web
						printf("Reception de la page web ... \n");
						length = 0;
						recu = 0;
						FILE *f = NULL;
						f = fopen(element->path,"w+");
						if(f != NULL)
						{
							while((recu = recv(*sockServeurWeb,reponseServeur,sizeof(reponseServeur),0)) > 0)
							{
								reponseServeur[recu] = '\0';
	                					fprintf(f,"%s",reponseServeur);
							}
							printf("PAGE WEB TELECHARGEE : \n%s\n",reponseServeur);
						}
					//Ajout dans le cache
						printf("Ajout de la page web au cache\n");
						addEltCache(element);
						fclose(f);
				}
			}
	}
	else
	{

	}
	printf("Fin de traitement.\n");
	//Libération de la mémoire
		closesocket(*structSock->socketClient);
		closesocket(*sockServeurWeb);
		free(sockServeurWeb);
		free(sockAddrServeurWeb);
		free(structSock->socketClient);
		free(structSock->sockAddrClient);
	//Libération semaphore
		sem_post(&semaphoreSocket);
	//Fin de traitement
		return NULL;
}
