#include "proxy.h"
#include <sys/stat.h>
//-------------------------------------------Corps Fonctions--------------------------------------//
//Fonction conversionChaineInt.
// Prend en paramètre une chaine.
//Cette fonction renvoie le nombre qui est dans la chaine.
int conversionChaineInt(char *chaine)
{
    int res = 0,i=0;
    while(chaine[i] != '\0')
    {
        res *=10;
        res += chaine[i] - '0';
        i++;
    }
    return res;
}
//Fonction recupHost.
// Prend en paramètre une requete http et un lien.
//Cette fonction renvoie le résultat directement dans lien.
void recupHost(char *requete, char *lien)
{
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
//Fonction recupTitrePage.
// Prend en paramètre une requete http et le nom.
//Cette fonction renvoie le nom de la page web voulu.
void recupTitrePage(char *requete, char *nom)
{
    char *pointeur, *repere;
    int i=0;
    pointeur =  strstr(requete,"GET");
    repere = strstr(requete,"HTTP/1.1");
    if(repere == NULL)
    {
	repere = strstr(requete,"HTTP/1.0");
    }
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
            nom[i] = '\0';
        }
    }
}
//Fonction recupExtension.
// Prend en paramètre le nom de la page.
//Cette fonction donne l'extension de la page
void recupExtension(char *nom,char *extension)
{
	int i = strlen(nom),y = 0;
    	while(nom[i] != '.')
	{
		i--;
	}
	i++;//Pour enlever le point
	while(nom[i] != '\0')
	{
		extension[y] = nom[i];
		i++;
		y++;
	}
	extension[3]='\0';
	printf("nom : %s => extension : %s\n",nom,extension);
}
//Fonction contenuFichier.
// Prend en paramètre un lien vers un fichier et la taille du contenu.
//Cette fonction renvoie le contenur d'un fichier.
char *contenuFichier(char *lienFichier,int* l)
{
    char *contenu = malloc(sizeof(char));
    FILE *file = NULL;
    file = fopen(lienFichier,"rb+");

		struct stat statbuf;
		int filesize;
		 stat(lienFichier,&statbuf);  /// the_file, le fichier a lir
    filesize = (long)statbuf.st_size;
    printf("Valeur:%d\n\n\n", filesize);

    if(file != NULL)
    {
        int t = 0;
        char c;
        do
        {
            c = fgetc(file);
            contenu[t] = c;
            t += 1;
				filesize--;
            contenu = realloc(contenu,sizeof(char)*t+1);
        }
        while(filesize);
        fclose(file);
        *l = t;
    }
    return contenu;
}
//Fonction nettoyageHTTP
//Prend en parametre une requete HTTP
//Cette fonction crée une requete HTTP plus simple
void nettoyageHTTP(char *requete)
{
    char nom[1024*1024];
    char host[1024*1024];
    recupTitrePage(requete,nom);
    recupHost(requete,host);
    sprintf(requete,"GET %s HTTP/1.1\r\nHost : %s\r\n\r\n",nom,host);
}
//Fonction logs
//Prend en parametre l'host, la page web, l'adresse IP du client
//Cette fonction ajoute une entrée dans les fichiers de logs
void logs(char *host, char*nom, char *ip)
{
    FILE *f = NULL;
    f = fopen("Conf/Logs/logs.csv","a+");
    time_t t = time(NULL);
    fprintf(f,"%s;%s;%s;%s",host,nom,ip,ctime(&t));
    fclose(f);
}
//-------------------------------------------Fonctions thread---------------------------------
//Fonction connexionClient qui prend en parametre une structure structSocketClient
//La fonction gère la connexion
void *client(void *arg)
{
	//Variables
    char extension[1024];
    char requeteHTTPClient[1024*1024];
    char *reponseServeur = calloc(1024,sizeof(char));
    char host[1024];
    char nomPage[1024];
    int length = 0, recu = 0;
	//Création de la nouvelle structure
    structSocketClient *structSock = (structSocketClient *) arg;
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
	//Extraction de l'host dans la requete HTTP reçu
    recupHost(requeteHTTPClient,host);
	//Extraction du nom de la page web demandée
    recupTitrePage(requeteHTTPClient,nomPage);
	//Extraction de l'extension de la page web demandée
    recupExtension(nomPage,extension);
	//Ajout dans les logs
    //logs(host,nomPage,inet_ntoa(structSock->sockAddrClient->sin_addr));
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
            char* pageErr = contenuFichier("PagesWeb/page404.html",&l);
            if(l > 0)
            {
                printf("Envoie de la page 404 ... ");
                if(send(*structSock->socketClient,pageErr,sizeof(char)*(l-2),0) <= 0)
                {
                    printf("\nERREUR envoie de la page erreur au client");
                }
                else
                {
                    printf("OK\n");
                }
                free(pageErr);
            }
            printf("Fin de traitement.\n");
            //Arret sockets
            closesocket(*structSock->socketClient);
            closesocket(*sockServeurWeb);
            //Libération de la mémoire
            free(sockServeurWeb);
            free(sockAddrServeurWeb);
            free(structSock->socketClient);
            free(structSock->sockAddrClient);
            //Libération semaphore
            sem_post(&semaphoreInterne);
            //Fin de traitement
            return NULL;
        }
	//Vérification téléchargement
	     int isDownload = isAuthorized(host,inet_ntoa(structSock->sockAddrClient->sin_addr),extension);
		printf("Valeur de retour de la fonction:%d\n\n", isDownload);
        if(isDownload == 3)
        {
            	printf("Telechargement detecte => REFUS\n");
        	//Envoie de la page au client interdisant le téléchargement
        	int l;
       		char* pageTelechargement = contenuFichier("PagesWeb/pageTelechargement.html",&l);
       		if(l > 0)
       		{
           		printf("Envoie de la page de téléchargement ... ");
            		if(send(*structSock->socketClient,pageTelechargement,sizeof(char)*(l-2),0) <= 0)
            		{
            		    	printf("\nERREUR envoie de la page telechargement au client.\n");
            		}
            		else
            		{
                		printf("OK\n");
            		}
            		free(pageTelechargement);
		}
            	printf("OK\nFin de traitement.\n");
            	//Arret des sockets
            	closesocket(*structSock->socketClient);
            	closesocket(*sockServeurWeb);
            	//Libération de la mémoire
            	free(sockServeurWeb);
            	free(sockAddrServeurWeb);
            	free(structSock->socketClient);
            	free(structSock->sockAddrClient);
            	//Libération semaphore
            	sem_post(&semaphoreInterne);
            	//Fin de traitement
            	return NULL;
        }
	//Vérification de l'accesibilité de la page web
        //int isAuth = isAuthorized(host,inet_ntoa(structSock->sockAddrClient->sin_addr));
        if(isDownload ==2)
        {
            printf("Lien non autorisé\n");
            //Envoie de la page au client interdisant cette page
            int l, e=0;
            int envoie = 0;
            char* pageAuth = contenuFichier("PagesWeb/pageAuth.html",&l);
            if(l > 0)
            {
                printf("Envoie de la page d'authorisation ... ");
                while(envoie < l)
                {
                    if((e=send(*structSock->socketClient,pageAuth + envoie,1024,0)) <= 0)
                    {
                        printf("\nERRREUR envoie de la page autorisation au client\n");
                        break;
                    }
                    envoie += e;
                }
            }
            printf("OK\nFin de traitement.\n");
            //Arret des sockets
            closesocket(*structSock->socketClient);
            closesocket(*sockServeurWeb);
            //Libération de la mémoire
            free(sockServeurWeb);
            free(sockAddrServeurWeb);
            free(structSock->socketClient);
            free(structSock->sockAddrClient);
            //Libération semaphore
            sem_post(&semaphoreInterne);
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
                printf("Page web trouvé dans le cache.");
                //Envoie de la page au client
                int l = 0,envoie = 0,e;
                //char* pageCache = contenuFichier("./tmp/coucou.gris.redu.1p.tmp",&l);
                char* pageCache = contenuFichier(elt->path,&l);
                //printf("%s\n",pageCache);
                printf(" Envoie de la page ...%d \n", l);
                //while(envoie < l)
                //{
                    if((e=send(*structSock->socketClient,pageCache,l,0)) <= 0)
                    {
                        printf("\nERRREUR envoie de la page web au client\n");
                        break;
                    }
							//printf("valeur de e:%d\n\n", e);
                   // envoie += e;
                //}
                printf("OK\n");
                free(pageCache);
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
                sockAddrServeurWeb->sin_port = htons(PORT_INTERNET_DEFAUT);
                //printf("%s\n",inet_ntoa(sockAddrServeurWeb->sin_addr));
                freeaddrinfo(structAddrIP);
                //Connexion au serveur web demandé
                printf("Connexion au serveur distant ..\n");
                connect(*sockServeurWeb,(SOCKADDR*)sockAddrServeurWeb,sockAddrServeurWebSize);
                //Nettoyage HTTP
					//printf("%s\n\n", requeteHTTPClient);
                //nettoyageHTTP(requeteHTTPClient);
                //Envoie de la requete HTTP au serveur web

                printf("Envoie de la requete http ... \n\n");
					printf("%s\n\n", requeteHTTPClient);


                if(send(*sockServeurWeb,requeteHTTPClient,sizeof(requeteHTTPClient),0) <= 0)
                {
                    printf("ERREUR d'envoie de la requete HTTP au serveur.\n");
                }
                else
                {
                    printf("OK\n");
                    //Configuration de la nouvelle entrée dans le cache
                    Cache_Elt* element = generate(nomPage);
                    //Réception de la requete HTTP et de la pageWeb du serveur web
                    char chaine[1024];
                    recu = 0;
                    int envoie = 0,e = 0;
                    FILE *f = NULL;
                    f = fopen(element->path,"wb");
							printf("Ouverture\n");
                    if(f != NULL)
                    {
                        //On accede a internet on decremente notre socket de connection externe
                        sem_wait(&semaphoreExterne);


		printf("Telecharge\n");
                        //On telecharge la page
                        while((recu = read(*sockServeurWeb,reponseServeur,1024)) > 0)
                        {
                            //length += recu;
									 reponseServeur[recu] = '\0';
									// int res = fprintf(f,"%s",reponseServeur);
										int res = fwrite(reponseServeur, sizeof(char)*recu, 1, f);
										//printf("Valeur de res:%d et valeur de recu: %d\n\n", res, recu);
                            //reponseServeur = realloc(reponseServeur,(1024 + length)*sizeof(char));
                        }
                        //On a fini de se connecter à internet on increment la socket de connection externe
                        sem_post(&semaphoreExterne);
                        printf("Téléchargement page ...\n");
								
                        //fprintf(f,"%s",reponseServeur);
                        printf("OK\n");


                        /*while(envoie < length)
                         {
                         if((e=send(*structSock->socketClient,reponseServeur + envoie,1024,0)) <= 0)
                         {
                         printf("\nERRREUR envoie de la page web au client\n");
                         break;
                         }
                         envoie += e;
                         }
                       	send(*structSock->socketClient,reponseServeur,sizeof(char)*length,0);
                        closesocket(*sockServeurWeb);*/
                    }
                    //Ajout dans le cache
                    printf("Ajout de la page web au cache ...\n");
                    addEltCache(element);
                    printf("OK\n");
                    fclose(f);
                   // break;
                }
            }
        }
	}
	else
	{
		printf("ERREUR Problème de socket.\n");
	}
	printf("Fin de traitement.\n");
	//Fermeture socket
    closesocket(*sockServeurWeb);
    closesocket(*structSock->socketClient);
	//Libération de la mémoire
    free(sockServeurWeb);
    free(sockAddrServeurWeb);
    free(structSock->socketClient);
    free(structSock->sockAddrClient);
	//Libération semaphore
    sem_post(&semaphoreInterne);
	//Fin de traitement
    return NULL;
}
