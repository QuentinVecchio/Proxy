//Bibliothèques à inclure
#include "header.h"

int main()
{
	int erreur = 0;
	int sock_err;
	//socket serveur
		SOCKADDR_IN sin;
		SOCKET sock;
		socklen_t recsize = sizeof(sin);
		int options = -1;
	//socket serveur demande
		SOCKADDR_IN serveurSin;
		SOCKET serveurSock;
	//Variables
	if(!erreur)
	{
		//Création socket
		sock = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(int));//Permet la réutilisation de la socket
		if(sock != INVALID_SOCKET)
		{
			//printf("La socket %d est maintenant ouverte en mode TCP/IP\n", sock);
			//Configuration
			sin.sin_addr.s_addr = htonl(INADDR_ANY);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(PORT);
			sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
			if(sock_err != SOCKET_ERROR)
			{
				while(1)
				{				
					//socket client
					SOCKADDR_IN *csin = malloc(sizeof(SOCKADDR_IN));
					SOCKET *csock = malloc(sizeof(SOCKET));					
					*csock = socket(AF_INET, SOCK_STREAM, 0);
					//printf("Fin de connexion\n");					
					sock_err = listen(sock,5);
					//printf("Listage du port %d ...\n", PORT);
					if(sock_err != SOCKET_ERROR)
					{
						//printf("Patientez pendant que le client se connecte sur le port %d ...\n", PORT);
						*csock = accept(sock, (SOCKADDR*)csin, &recsize);
						structSocketClient *structSock = malloc(sizeof(structSocketClient));
						structSock->csock = *csock;
						pthread_t *threadConnexion = malloc(sizeof(pthread_t));
						pthread_create(threadConnexion,NULL,client,(void *)structSock);	
					}
					else
						perror("listen");
				}
			}
			else
				perror("bind");
			//printf("Fermeture de la socket client.\n");
			//printf("Fermeture de la socket serveur.\n");
			closesocket(sock);
			//printf("Fin de programme...\n");
		}
		else
			perror("socket");
	}
	return EXIT_SUCCESS;
}
