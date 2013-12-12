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

/**
*	Fonction qui renvoit un pointeur vers un élément de la liste si la fonction fonc la repère
*	@params Liste* l, la liste où s'effectue la recherche
*	@params void* elt, l'élément qui sert de comparaison
*	@params int(*fonc)(), la fonction de comparaison
*			@params void* elt, l'élément de comparaison
*			@params void* tmp, l'élément a comparer
*			@return true, si l'élément correspond
*			@return false, sinon
*	@return void*, le pointeur vers l'élément s'il se trouve dans la liste
*	@return NULL, sinon
*/
void* getElt(Liste* l, void* elt, int(*fonc)());


#endif
