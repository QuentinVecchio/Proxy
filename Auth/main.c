#include <stdio.h>
#include <stdlib.h>
#include "Auth.h"

void afficheRegle(void* arg){
	Auth_Regle* regle = (Auth_Regle*) arg;
	printf("Affichage:\n lien: %s\nadresse: %s\nstatus: %d\n", regle->link, regle->address, regle->status);
}


int main(){
	printf("bienvenue\n");
	Auth_Conf a_C;

	char lB[] = "../Conf/listeBlanche.txt";
	char lN[] = "../Conf/listeNoire.txt";
	char lR[] = "../Conf/listeRegle.txt";

	a_C.listeBlanche = lB;
	a_C.listeNoire = lN;
	a_C.listeRegle = lR;

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
	printf("Affichage\n");
	affiche(&Auth_Var_Liste_Regle, &afficheRegle);



}
