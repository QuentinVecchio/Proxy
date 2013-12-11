#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "cache.h"

void initCache(){
	printf("Initialisation du module de cache\n");

	if(mkdir("./tmp", 0777)){
		perror("Erreur lors de la création du dossier\n");	
	}else{
		printf("Dossier de cache crée avec succès\n");
	}

	initListe(&Cache_Var_Liste_Cache);

}

void closeCache(){
	if(remove_directory("./tmp"))perror("Suppression dossier tmp");
	else printf("Dossier supprimé avec succès\n");

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

