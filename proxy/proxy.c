#include "proxy.h"

int main()
{
	//Variables
		int erreur = 0;
		int sock_err;
		int options = -1;
	//Socket du serveur proxy
		SOCKET sockProxy;
		SOCKADDR_IN sockAddrProxy;
		socklen_t sockAddrProxySize = sizeof(sockAddrProxy);
	//Initialisation du semaphore limitant le nombre de connexion sur le proxy
		sem_init(&semaphoreSocket,0,M);
	if(!erreur)
	{
		//Création socket proxy
			sockProxy = socket(AF_INET, SOCK_STREAM, 0);
			setsockopt(sockProxy, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(int));
		if(sockProxy != INVALID_SOCKET)
		{
			printf("La socket proxy %d est configurée\n",sockProxy);	
			//Configuration de la socket proxy
				sockAddrProxy.sin_addr.s_addr = htonl(INADDR_ANY);
				sockAddrProxy.sin_family = AF_INET;
				sockAddrProxy.sin_port = htons(PORT);
			//Liaison Socket proxy et configuration
				sock_err = bind(sockProxy,(SOCKADDR*)&sockAddrProxy,sockAddrProxySize);
			if(sock_err != SOCKET_ERROR)
			{
				//Proxy qui démarre
				while(1)
				{
					//Création socket cliente
						SOCKET *sockClient = malloc(sizeof(SOCKET));
						SOCKADDR_IN *sockAddrClient = malloc(sizeof(SOCKADDR_IN));
						socklen_t sockAddrClientSize = sizeof(*sockAddrClient);
					//Création socket cliente
						*sockClient = socket(AF_INET, SOCK_STREAM, 0);
					//Listage du port
						printf("Listage du port %d, en attente de connexion ...\n",PORT);
						sock_err = listen(sockProxy,M);
					if(sock_err != SOCKET_ERROR)
					{
						*sockClient = accept(sockProxy, (SOCKADDR*)sockAddrClient, &sockAddrClientSize);
						structSocketClient *structSock = malloc(sizeof(structSocketClient));
						structSock->socketClient = sockClient;
						structSock->sockAddrClient = sockAddrClient;
						pthread_t *threadConnexion = malloc(sizeof(pthread_t));
						pthread_create(threadConnexion,NULL,client,(void *)structSock);
					}
					else
					{

					}
				}
			}
			else
			{

			}
		}
		else
		{

		}
	}
	else
	{

	}
	return 0;
}
