#include "listeGenerique.h"
#include <stdlib.h>
#include <stdio.h>

void initListe(Liste* l){
	(*l) = NULL;
}

void addElt(Liste* l, void* a){
	if((*l) == NULL){
		(*l) = malloc (sizeof(Element));
		(*l)->courant = a;
		(*l)->suivant = NULL;
	}else{
		if((*l)->suivant == NULL){
			(*l)->suivant = malloc (sizeof(Element));
			(*l)->suivant->courant = a;
			(*l)->suivant->suivant = NULL;
		}else{
			addElt(&(*l)->suivant, a);
		}
	}
}

void affiche(Liste* l,  void (*fonc)(void*)){
	if((*l) != NULL){
		(*fonc)((*l)->courant);
		affiche(&(*l)->suivant, fonc);
	}
}

int recherche(Liste* l,void* elt, int (*fonc)()){
	if((*l) != NULL){
		if((*fonc)(elt,(*l)->courant)) return 1;
		else return recherche(&((*l)->suivant),elt,fonc);
	}

	return 0;
}

void deleteListe(Liste* l){
	if((*l) != NULL){
		deleteListe(&(*l)->suivant);
		free((*l)->suivant);
	}
}

void* getElt(Liste* l, void* elt, int(*fonc)()){
	if((*l) != NULL){
		if((*fonc)(elt, (*l)->courant)) return (*l)->courant;
		else return getElt(&((*l)->suivant), elt, fonc);
	}

	return NULL;
}

void keepElt(Liste* l, int(*fonc)()){
	if((*l) !=NULL){
		if(fonc((*l)->courant)){
			Element* elt = (*l);
			(*l) = (*l)->suivant;
			free(elt);
		}else{
			Liste* tmp = &(*l)->suivant;
			Liste* avant = l;
			do{
				printf("oui\n");
				if(fonc((*tmp)->courant)){
					printf("Efface\n");
					Element* elt = (*tmp);
					(*avant)->suivant =(*tmp)->suivant;
					free(elt);
				}
				avant = &(*avant)->suivant;
				tmp = &(*tmp)->suivant;
			}while((*tmp)->suivant);
		}
	}
}

