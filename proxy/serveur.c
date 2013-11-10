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

	//socket client
	SOCKADDR_IN csin;
        SOCKET csock;
        socklen_t crecsize = sizeof(csin);
	
	//socket serveur demande
	SOCKADDR_IN serveurSin;
	SOCKET serveurSock;
	socklen_t reServeurSize = sizeof(serveurSin);
	
	//Variables
	if(!erreur)
	{
		//Création socket
		sock = socket(AF_INET, SOCK_STREAM, 0);

		if(sock != INVALID_SOCKET)
		{
			printf("La socket %d est maintenant ouverte en mode TCP/IP\n", sock);
			//Configuration
			sin.sin_addr.s_addr = htonl(INADDR_ANY);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(PORT);
			sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
			if(sock_err != SOCKET_ERROR)
			{
				sock_err = listen(sock,5);
				printf("Listage du port %d ...\n", PORT);
				if(sock_err != SOCKET_ERROR)
				{
					printf("Patientez pendant que le client se connecte sur le port %d ...\n", PORT);
					csock = accept(sock, (SOCKADDR*) &csin, &recsize);
					printf("Un client se connecte avec la socket %d de %s:%d\n",csock,inet_ntoa(csin.sin_addr), htons(csin.sin_port));
					recv(csock,host,sizeof(host),0);
					recupHost(host,lien);
					printf("ok\n");
					//Configuration socket serveur demandé
					serveurSock = socket(AF_INET, SOCK_STREAM, 0);
					if(serveurSock != INVALID_SOCKET)
					{
						//Configuration
						printf("ok\n");
						struct hostent *monHost;
						monHost = gethostbyname(lien);
					 	memcpy((char *)&serveurSin.sin_addr,(char *)monHost->h_addr,monHost->h_length);
						serveurSin.sin_family = AF_INET;
						serveurSin.sin_port = htons(80);
						printf("%s\n",inet_ntoa(serveurSin.sin_addr));
						connect(serveurSock,(SOCKADDR*)&serveurSin, sizeof(serveurSin));
						send(serveurSock,host,sizeof(host),0);
						recv(serveurSock,pageWeb,sizeof(pageWeb),0);
						printf("Fermeture de la socket Serveur Web.\n");
						closesocket(serveurSock);
						FILE *f;
						f= fopen("test.php","a+");
						fprintf(f,"%s",pageWeb);
						//printf("%s",pageWeb);
						send(csock,pageWeb,sizeof(pageWeb),0);
					}
					else
						perror("connexion serveur demande");
				}
				else
					perror("listen");
			}
			else
				perror("bind");
			printf("Fermeture de la socket client.\n");
			closesocket(csock);
			printf("Fermeture de la socket serveur.\n");
			closesocket(sock);
			printf("Fin de programme...\n");
		}
		else
			perror("socket");
	}
	return EXIT_SUCCESS;
}
