#ifndef AUTH_H 
#define AUTH_H

#include "../ListeGenerique/listeGenerique.h"
#include <pthread.h>
typedef struct{
	char* listeBlanche;
	char* listeNoire;
}Auth_Conf;


#define NB_THREAD 2
typedef struct{
	char* lien;
	int estDansListe[NB_THREAD];
	Liste listeRecherche[NB_THREAD];
	void* fonctionCmp[NB_THREAD];

	int index;
	pthread_mutex_t m_index;
}Auth_Search;

typedef struct{
	char *lien;
}Auth_Lien;


Auth_Conf Auth_Var_Conf;

Liste Auth_Var_Liste_Blanche;

Liste Auth_Var_Liste_Noire;


void init(Auth_Conf conf);

void load();

void loadListe(char* lien, Liste* l);

int search(char* lien);


#endif
