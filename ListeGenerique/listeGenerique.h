#ifndef LISTEGENERIQUE_H
#define LISTEGENERIQUE_H

typedef struct Element{
	void* courant;
	struct Element* suivant;
} Element;


typedef Element* Liste;


/**
*	Initialise la liste
*	@params Liste* l, la liste à initialiser
*/
void initListe(Liste* l);

/**
*	Ajout un élément à la fin de la liste
*	@params Liste* l, la liste
*	@params void* a, l'élément à ajouter
*/
void addElt(Liste* l, void* a);

/**
*	Affiche la liste
*	@params Liste* l , la liste a afficher
*	@params void (*fonc)(void*), la fonction d'affichage
*/
void affiche(Liste* l,  void (*fonc)(void*));

/**
*	Recherche un élément dans la liste
*	@params Liste* l , la liste dans laquelle chercher
*	@params void (*fonc)(void*), la fonction d'analyser
*			@return true, la fonction doit renvoyer true si l'élement respecte les conditions
*			@return false, sinon
*	@return true si l'élément est trouvé dans la liste
@	@return false sinon
*/
int recherche(Liste* l,void* elt, int (*fonc)());

/**
*	Libère la liste
*	@params Liste* l, la liste a supprimer
*/
void deleteListe(Liste* l);



#endif
