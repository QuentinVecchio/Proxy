#ifndef AUTH_H 
#define AUTH_H

#include "../ListeGenerique/listeGenerique.h"
#include <pthread.h>

/**
*	Ensemble des données de configurations du module Auth
*	On y retrouve le chemin vers la liste blanche et la liste noire
*
*/
typedef struct{
	char* listeBlanche;
	char* listeNoire;
	char* listeRegle;
}Auth_Conf;

/**
*	NB_THREAD correspond au nombre de recherche simultannée que l'on fait des les fichiers
*	Pour le moment 2, c'est à dire listeBlanche et listeNoire, a terme on rajoutera listeRegleParticuliere
*/
#define NB_THREAD 2

/**
*	Structure pour rechercher une correspondance
*	On y retrouve:
*		- lien, le lien a rechercher dans les listes
*		- int
*	
*/
typedef struct{
	char* lien;
	int estDansListe[NB_THREAD];
	Liste listeRecherche[NB_THREAD];
	void* fonctionCmp[NB_THREAD];

	int index;
	pthread_mutex_t m_index;
}Auth_Search;

/**
*	Structure des règles pour un ordinateur précis
*/
typedef struct{
	char* link;
	char* address;
	int 	status;
}Auth_Regle;



/**
*	Variable de configuration du module Auth
*/
Auth_Conf Auth_Var_Conf;

/**
*	Liste blanche des adresses autorisées
*/
Liste Auth_Var_Liste_Blanche;

/**
*	Liste noire des adresses non autorisées
*/
Liste Auth_Var_Liste_Noire;

/**
*	Liste des règles
*/
Liste Auth_Var_Liste_Regle;


/**
*	Inititalisation du module Auth
*/
void init(Auth_Conf conf);

/**
*	Chargement des listes en mémoire
*	@return 1, si problème lors du chargement
*	@return 0, sinon
*/
int load();

/**
*	Chargement d'une liste donnée en mémoire
*	@return 1, si problème lors de l'ouverture du fichier
*	@return 0, sinon
*/
int loadListe(char* lien, Liste* l);

/**
*	Chargement des règles
*	@return 1, si problème lors de l'ouverture du fichier
*	@return 0, sinon
*/
int loadRules(char* lien, Liste* l);


/**
*	Recherche d'un lien dans les listes
*/
int search(char* lien);

/**
*	Fonction de recherche dans les listes
*	Parcours selon un index fixé dans la structure, la liste correspondante
*/
void* thread_search(void* arg);

/**
*	Compare deux chaines
*	@params void* valeur
*	@params void* elt
*	@return 1, si les deux chaines sont égales
*	@return 0, si les deux chaines sont différentes
*/
int cmp_lien(void* valeur, void* elt);

/**
*	Test si le lien est dans une des listes
*	@return 1, si le lien est autorisé
*	@return 0, sinon
*/
int isAuthorized(char* lien);

#endif
