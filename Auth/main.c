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
	printf("********************************\n");
	load();
}
