#include "Proxy/proxy.h"

int main(int argc, char *argv[])
{
	//Initialisation du programme avec les parametres
    //Init port
    int i;
    port = PORT_DEFAUT;
    maxConnexionInterne = MAX_CONNEXION_INTERNE_DEFAUT;
    maxConnexionExterne =  MAX_CONNEXION_EXTERNE_DEFAUT;
    tps = TPS_DEFAUT;
    for(i=1;i<argc;i++)
    {
        char *p;
        p = strstr(argv[i],"-p");
        if(p==NULL)
        {
            p = strstr(argv[i],"-CI");
            if(p==NULL)
            {
                p = strstr(argv[i],"-CE");
                if(p==NULL)
                {
                    p = strstr(argv[i],"-t");
                    if(p==NULL)
                    {
                        p = strstr(argv[i],"-h");
                        if(p==NULL)
                        {
                            printf("ERREUR les paramètres spécifiés sont incorrects, tapez proxy -h pour avoir de l'aide\n");
                            return 0;
                        }
                        else
                        {
                            printf("Aide :\n \tExemple d'utilisation : proxy -p65000 -CE10 -CI10 -t30\n\tOptions : \n\t\t-p : Port du proxy\n\t\t-CE : Nombre max de congestion externe\n\t\t-CI : Nombre de congestion interne\n\t\t-t : Nombre de seconde de mise en cache d'un fichier\n\t\t-h : Voir l'aide du programme PROXY\n");
                            printf("Programme développé par Matthieu et Quentin\n");
                            printf("Ce proxy ne gère que le htttp sur une connexion du port 80\n");
                            return 0;
                        }
                    }
                    else
                    {
                        p +=2;
                        tps = conversionChaineInt(p);
                    }
                }
                else
                {
                    p+=3;
                    maxConnexionExterne = conversionChaineInt(p);
                }
            }
            else
            {
                p+=3;
                maxConnexionInterne = conversionChaineInt(p);
            }
        }
        else
        {
            p+=2;
            port = conversionChaineInt(p);
        }
    }
	//Variables
    int erreur = 0;
    int sock_err;
    int options = -1;
	//Configuration proxy
    printf("Bienvenue sur le PROXY v1.0 de Matthieu et Quentin.\nVous trouverez de la documentation dans le dossier /README\nConfiguration :\n");
    printf("Port proxy : %d\n",port);
    printf("Port Internet : %d\n",PORT_INTERNET_DEFAUT);
    printf("Nombre de congestions interne : %d\n",maxConnexionInterne);
    printf("Nombre de congestions externe : %d\n",maxConnexionExterne);
    printf("Temps de mise en cache : %d\n",tps);
	//Configuration variables authenficication
    printf("Configuration service authentification ...\n");
    sleep(1);
    Auth_Conf *a_C = malloc(sizeof(Auth_Conf));
    char lN[] = "../Conf/listeNoire.txt\0";
    char lB[] = "../Conf/listeBlanche.txt\0";
    char lR[] = "../Conf/listeRegle.txt\0";
    a_C->listeBlanche = lB;
    a_C->listeNoire = lN;
    a_C->listeRegle = lR;
    init(*a_C);
    if(load())
    {
        printf("ERREUR Chargement authentification a échoué ... ARRET DU PROGRAMME\n");
        return 0;
    }
	//Configuration du cache
    printf("Configuration service cache ...\n");
    sleep(1);
    char tmp[] = "./tmp";
    initCache(10, tmp);
    Cache_Elt* e = malloc(sizeof(Cache_Elt));
	//Socket du serveur proxy
    SOCKET sockProxy;
    SOCKADDR_IN sockAddrProxy;
    socklen_t sockAddrProxySize = sizeof(sockAddrProxy);
	//Initialisation du semaphore limitant le nombre de connexion sur le proxy
    sem_init(&semaphoreInterne,0,maxConnexionInterne);
    sem_init(&semaphoreExterne,0,maxConnexionExterne);
	if(!erreur)
	{
		printf("Configuration socket proxy ...\n");
		sleep(1);
		//Création socket proxy
        sockProxy = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(sockProxy, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(int));
		if(sockProxy != INVALID_SOCKET)
		{
			printf("La socket proxy %d est configurée\n",sockProxy);
			//Configuration de la socket proxy
            sockAddrProxy.sin_addr.s_addr = htonl(INADDR_ANY);
            sockAddrProxy.sin_family = AF_INET;
            sockAddrProxy.sin_port = htons(port);
			//Liaison Socket proxy et configuration
            sock_err = bind(sockProxy,(SOCKADDR*)&sockAddrProxy,sockAddrProxySize);
			if(sock_err != SOCKET_ERROR)
			{
				//Proxy qui démarre
				while(1)
				{
                    //Gestion du semaphore on la decremente
                    sem_wait(&semaphoreInterne);
					//Création socket cliente
                    SOCKET *sockClient = malloc(sizeof(SOCKET));
                    SOCKADDR_IN *sockAddrClient = malloc(sizeof(SOCKADDR_IN));
                    socklen_t sockAddrClientSize = sizeof(*sockAddrClient);
					//Création socket cliente
                    *sockClient = socket(AF_INET, SOCK_STREAM, 0);
					//Listage du port
                    sock_err = listen(sockProxy,maxConnexionInterne);
					if(sock_err != SOCKET_ERROR)
					{
						*sockClient = accept(sockProxy, (SOCKADDR*)sockAddrClient, &sockAddrClientSize);
						structSocketClient *structSock = malloc(sizeof(structSocketClient));
						structSock->socketClient = sockClient;
						structSock->sockAddrClient = sockAddrClient;
						structSock->a_C = *a_C ;
						structSock->e = e;
						pthread_t *threadConnexion = malloc(sizeof(pthread_t));
						pthread_create(threadConnexion,NULL,client,(void *)structSock);
					}
					else
					{
						printf("ERREUR Impossible de traiter avec ce client\n");
					}
				}
			}
			else
			{
				printf("ERREUR Socket proxy BIND ... ARRET DU PROGRAMME\n");
			}
		}
		else
		{
			printf("ERREUR Creation socket ... ARRET DU PROGRAMME\n");
		}
	}
	else
	{
		printf("ERREUR INTERNE AU PROBLEME ... ARRET DU PROGRAMME\n");
	}
	closeCache();
	return 0;
}

