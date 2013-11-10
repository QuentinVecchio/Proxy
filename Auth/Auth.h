#ifndef AUTH_H 
#define AUTH_H

#include "../ListeGenerique/listeGenerique.h"

typedef struct{
	char* listeBlanche;
	char* listeNoire;
}Auth_Conf;

Auth_Conf Auth_Var_Conf;

Liste Auth_Var_Liste_Blanche;

Liste Auth_Var_Liste_Noire;


void init(Auth_Conf conf);

void load();

void loadListe(char* lien, Liste* l);

#endif
