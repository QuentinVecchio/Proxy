#include <stdio.h>
#include <stdlib.h>
#include "Auth.h"

int main(){
	printf("bienvenue\n");
	Auth_Conf a_C;

	char lB[] = "../Conf/listeBlanche.txt";
	char lN[] = "../Conf/listeNoire.txt";

	a_C.listeBlanche = lB;
	a_C.listeNoire = lN;

	init(a_C);

	if(load()){
		printf("Erreur du chargement\n");
		return 0;
	}

	int isAuth = isAuthorized("http://www.jeuxvideo.fr/");
	if(isAuth){
		printf("Lien autorisé\n");
	}else{
		printf("Lien non autorisé\n");
	}
}
