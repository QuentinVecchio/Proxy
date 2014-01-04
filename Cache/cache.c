#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "cache.h"

void initCache(int limit, char* tmp){
	printf("Initialisation du module de cache\n");
	Cache_Var_Conf.limit = limit;
	Cache_Var_Conf.tmp = tmp;


	if(mkdir(Cache_Var_Conf.tmp, 0777)>0){
		perror("Erreur lors de la création du dossier\n");	
	}else{
		printf("Dossier de cache crée avec succès\n");
	}

	printf("Initialisation de la liste de cache\n");
	initListe(&Cache_Var_Liste_Cache);

	pthread_mutex_init(&m_liste, NULL);

	sem_init(&s_liste, 0, Cache_Var_Conf.limit);

	srand(time(NULL));
	
	pthread_create(&t_refresh, NULL, refresh, NULL);
}


void closeCache(){

	pthread_mutex_lock(&m_liste);
	int i = Cache_Var_Conf.limit;
	while(i){
		sem_wait(&s_liste);
		i--;
	}
	printf("Suppression de la liste\n");
	deleteListe(&Cache_Var_Liste_Cache);

	i = Cache_Var_Conf.limit;
	while(i){
		sem_post(&s_liste);
		i--;
	}
	pthread_mutex_unlock(&m_liste);
	pthread_join(t_refresh, NULL);

	if(remove_directory(Cache_Var_Conf.tmp))perror("Suppression dossier tmp");
	else printf("Dossier supprimé avec succès\n");

}
int keepEltInCache(void* params){
	Cache_Elt* elt = (Cache_Elt*) params;
	//printf("url:%s, path: %s, timestamps:%d\n", elt->url, elt->path, elt->timestamp);
	pthread_mutex_lock(&elt->m);
		remove(elt->path);
	pthread_mutex_unlock(&elt->m);

	return  time(NULL)>elt->timestamp+10;
}


void* refresh(void* params){
	printf("Initialiation du refresh\n");
	while(1){
		sleep(2);
		int i = Cache_Var_Conf.limit;
		while(i){
			sem_wait(&s_liste);
			i--;
		}
		keepElt(&Cache_Var_Liste_Cache, keepEltInCache);
		i = Cache_Var_Conf.limit;
		while(i){
			sem_post(&s_liste);
			i--;
		}
		pthread_mutex_unlock(&m_liste);
	}
}


Cache_Elt* generate(char* url){
	static id = 0;
	Cache_Elt* elt = malloc(sizeof(Cache_Elt));
	elt->url = url;
	
	elt->path = malloc(sizeof(char)*2048);
	sprintf(elt->path, "%d_%d%s", (int)time(NULL),id, ".tmp");
	elt->timestamp = time(NULL);
	pthread_mutex_init(&elt->m, NULL);
	
	id = (id+1)%20;
	return elt;
}


void addEltCache(Cache_Elt* elt){
	int i = Cache_Var_Conf.limit;
	while(i){
		sem_wait(&s_liste);
		i--;
	}
	Cache_Elt* tmp = malloc(sizeof(Cache_Elt));
	tmp = elt;
	addElt(&Cache_Var_Liste_Cache, (void*) tmp);

	i = Cache_Var_Conf.limit;
	while(i){
		sem_post(&s_liste);
		i--;
	}
}

int cmp_cache(void* elt, void* cmp){
	Cache_Elt* e = (Cache_Elt*) elt;
	Cache_Elt* c = (Cache_Elt*) cmp;

	if(!strcmp(e->url, c->url)) return 1;
	else return 0;
}


Cache_Elt* isInCache(char* url){
	Cache_Elt elt;
	elt.url = url;
	void* t = getElt(&Cache_Var_Liste_Cache, (void*) &elt, cmp_cache);
	Cache_Elt* res;
	if(t==NULL) res= NULL;
	else res = (Cache_Elt*) t;
	return res;
}


int remove_directory(char const *name)
{
    DIR *directory;          
    struct dirent *entry;    
    struct stat file_stat;   

    char buffer[1024] = {0};

    directory = opendir(name);
    if ( directory == NULL ) {
       perror("Erreur dossier\n");
        return 0;
    }

    while ( (entry = readdir(directory)) != NULL ) {

        if ( strcmp(entry->d_name, ".") == 0 ||
             strcmp(entry->d_name, "..") == 0 ) {
            continue;
        }

        snprintf(buffer, 1024, "%s/%s", name, entry->d_name);

        stat(buffer, &file_stat);

        if ( S_ISREG(file_stat.st_mode) ) {
            remove(buffer);
        }
        else if ( S_ISDIR(file_stat.st_mode) ) {
            remove_directory(buffer);
        }
    }

    closedir(directory);

    return remove(name);

}

