#ifndef PROXY_H
#define PROXY_H
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
	#include "../Auth/auth.h"
//Bibliothèques Liste
	#include "../ListeGenerique/listeGenerique.h"
//Bibliothèques autres
	#include <time.h>
	#include <string.h>
//Variables constantes
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket(s) close (s)
	#define PORT_INTERNET_DEFAUT 80
	#define PORT_DEFAUT 65000
	#define MAX_CONNEXION_INTERNE_DEFAUT 10;
	#define MAX_CONNEXION_EXTERNE_DEFAUT 10;
	#define TPS_DEFAUT 10;
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
	int port;
	int maxConnexionInterne;
	int maxConnexionExterne;
	int tps;
	sem_t semaphoreInterne;
	sem_t semaphoreExterne;
//-------------------------------------------En tetes Fonctions--------------------------------------//
	//Fonction conversionChaineInt.
    // Prend en paramètre une chaine.
    //Cette fonction renvoie le nombre qui est dans la chaine.
    int conversionChaineInt(char *chaine);

    //Fonction recupHost.
	// Prend en paramètre une requete http et un lien.
	//Cette fonction renvoie le résultat directement dans lien.
    void recupHost(char *requete, char *lien);

    //Fonction recupTitrePage.
    // Prend en paramètre une requete http et le nom.
    //Cette fonction renvoie le nom de la page web voulu.
    void recupTitrePage(char requete[], char nom[]);

    //Fonction recupExtension.
    // Prend en paramètre le nom de la page web.
    //Cette fonction donne  l'extension de la page web demandée
    void recupExtension(char nom[],char extension[])

    //Fonction contenuFichier.
    // Prend en paramètre un lien vers un fichier et la taille du contenu.
    //Cette fonction renvoie le contenur d'un fichier.
    char *contenuFichier(char *lienFichier,int* l);

    //Fonction nettoyageHTTP
	//Prend en parametre une requete HTTP
	//Cette fonction crée une requete HTTP plus simple
    void nettoyageHTTP(char *requete);

	//Fonction logs
	//Prend en parametre l'host, la page web, l'adresse IP du client
	//Cette fonction ajoute une entrée dans les fichiers de logs
    void logs(char *host, char*nom, char *ip);
//-------------------------------------------Fonctions thread---------------------------------
	//Fonction connexionClient qui prend en parametre une structure structSocketClient
	//La fonction gère la connexion
void *client(void *arg);
#endif
