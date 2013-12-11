#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "cache.h"

void initCache(int limit){
	printf("Initialisation du module de cache\n");
	Cache_Var_Conf.limit = limit;


	if(mkdir("./tmp", 0777)){
		perror("Erreur lors de la création du dossier\n");	
	}else{
		printf("Dossier de cache crée avec succès\n");
	}

	printf("Initialisation de la liste de cache\n");
	initListe(&Cache_Var_Liste_Cache);

	pthread_mutex_init(&m_liste, NULL);

	sem_init(&s_liste, 0, Cache_Var_Conf.limit);

	
	pthread_create(&t_refresh, NULL, refresh, NULL);
}

void closeCache(){
	if(remove_directory("./tmp"))perror("Suppression dossier tmp");
	else printf("Dossier supprimé avec succès\n");


	pthread_mutex_lock(&m_liste);
	int i = Cache_Var_Conf.limit;
	while(i){
		sem_wait(&s_liste);
		i--;
	}
	deleteListe(&Cache_Var_Liste_Cache);

	i = Cache_Var_Conf.limit;
	while(i){
		sem_post(&s_liste);
		i--;
	}
	pthread_mutex_unlock(&m_liste);
	pthread_join(t_refresh, NULL);

}

void* refresh(void* params){
	printf("Initialiation du refresh\n");
	while(1){
		sleep(2);
		printf("Je m'active\n");
		int i = Cache_Var_Conf.limit;
		while(i){
			sem_wait(&s_liste);
			i--;
		}
		printf("Je travaille\n");

		i = Cache_Var_Conf.limit;
		while(i){
			sem_post(&s_liste);
			i--;
		}
		pthread_mutex_unlock(&m_liste);
		printf("J'ai fini\n");
	}
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

