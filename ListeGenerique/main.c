#include <stdio.h>
#include <stdlib.h>
#include "listeGenerique.h"

void f_affiche(void* elt){
	char* tmp = (char*) elt;
	printf("Valeur:%s\n", tmp);
}

int keep(void*elt){
	char* tmp= (char*) elt;
	return !strcmp(tmp, "toto");
}

int f_recherche(void* valeur, void* elt){
	int a = *((int*) &elt);
	int b = *((int*) &valeur);

	return a == b;
}

int main(){
	printf("Bienvenue\n");

	Liste l;

	initListe(&l);

	char t1[] = "toto";
	char t2[] = "tata";
	char t3[] = "titi";

	addElt(&l, (void*) &t1);
	addElt(&l, (void*) &t2);
	affiche(&l, &f_affiche);
	printf("\n");
	keepElt(&l, keep);
	affiche(&l, &f_affiche);
	printf("\n");

	return EXIT_SUCCESS;
}
