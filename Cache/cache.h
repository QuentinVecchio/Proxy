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
*	Fonction qui détermine si l'élément dans le cache est obsolète ou non
*	@params void* params, l'élément courant a tester
*	@return 0 si l'élément est a garder
*	@return 1 si l'élément doit être supprimé de la liste
*/
int keepEltInCache(void* params);

/**
*	Fonction qui raffraifit le cache
*	Supprime les données en cache une fois le temps écoulé
*/
void* refresh(void* params);

/**
*	Génération d'une entrée du cache
*	@params char* url, l'url de la page qui sera mise en cache
*/
Cache_Elt* generate(char* url);

/**
*	Fonction d'ajout d'un élément dans le cache
*	Cette fonction n'ajoute que l'information
*	Elle n'écrit en aucun cas sur le disque !
*	@params Cache_Elt, l'élément à ajouter
*/
void addEltCache(Cache_Elt* elt);

/**
*	Cette fonction permet de savoir si l'élément est dans le cache ou non
*	@params char* url, url du fichier distant
*	@return null, si l'élément ne se trouve pas dans le cache
*	@return CacheElt*, sinon
*/
Cache_Elt* isInCache(char* url);



/**
*	Suppression récursive du dossier
*	@return -1, si le dossier n'est pas supprimé
*/
int remove_directory(char const *name);
#endif
