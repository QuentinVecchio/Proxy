#ifndef CACHE_H
#define CACHE_H

#include "../ListeGenerique/listeGenerique.h"
#include "pthread.h"
#include "semaphore.h"

/**
*	Structure de configuration du module de Cache
*/
typedef struct{
	int limit;
	char *tmp;
}Cache_Conf;

/**
*	Elément dans le cache
*/
typedef struct{
	char* url;
	char* path;
	int timestamp;
	pthread_mutex_t m;
}Cache_Elt;

/**
*	Configuration du module de cache
*/
Cache_Conf Cache_Var_Conf;

/**
*	Liste des éléments en cache
*/
Liste Cache_Var_Liste_Cache;

/**
*	Accès à la liste pour modification
*/
pthread_mutex_t m_liste;

/**
*	Limite le nombre d'accès à la liste en simultanné
*/
sem_t s_liste;

/**
*	Thread qui raffraichit le cache
*/
pthread_t t_refresh;

/**
*	Initialisation du module de cache
*	@params int limit, limitation du nombre d'accès simultanné au cache
*	@params char* tmp, chemin vers le dossier tmp
*/
void initCache(int limit, char* tmp);

/**
*	Fermeture du module de cache
*/
void closeCache();

/**
*	Fonction qui raffraifit le cache
*	Supprime les données en cache une fois le temps écoulé
*/
void* refresh(void* params);

/**
*	Génération d'une entrée du cache
*/
Cache_Elt generate(char* url);

/**
*	Fonction d'ajout d'un élément dans le cache
*	Cette fonction n'ajoute que l'information
*	Elle n'écrit en aucun cas sur le disque !
*	@params Cache_Elt, l'élément à ajouter
*/
void addEltCache(Cache_Elt elt);

/**
*	Suppression récursive du dossier
*	@return -1, si le dossier n'est pas supprimé
*/
int remove_directory(char const *name);
#endif
