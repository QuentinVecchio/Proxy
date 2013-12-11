#ifndef CACHE_H
#define CACHE_H

#include "../ListeGenerique/listeGenerique.h"

/**
*	Liste des éléments en cache
*/
Liste Cache_Var_Liste_Cache;


/**
*	Initialisation du module de cache
*/
void initCache();

/**
*	Fermeture du module de cache
*/
void closeCache();

/**
*	Suppression récursive du dossier
*	@return -1, si le dossier n'est pas supprimé
*/
int remove_directory(char const *name);
#endif
