#include <stdio.h>
#include <stdlib.h>
#include "listeGenerique.h"

void f_affiche(void* elt){
	int a = *((int*) &elt);
	printf("Valeur : %d \n", a);
}

int f_recherche(void* valeur, void* elt){
	int a = *((int*) &elt);
	int b = *((int*) &valeur);

	return a == b;
}

int main(){
	printf("Bienvenue\n");

	int a = 2, b=3, c = 4;
	Liste l;
	initListe(&l);

	/*addElt(&l, (void*) a);
	addElt(&l, (void*) b);
	addElt(&l, (void*) c);
	affiche(&l, &f_affiche);
	printf("\n");*/
	int d = 5;
	if(recherche(&l, (void*) d, &f_recherche)) printf("oui\n");
	else printf("non\n");

	return EXIT_SUCCESS;
}
