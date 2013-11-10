#include "Auth.h"
#include "stdio.h"
#include "stdlib.h"

void init(Auth_Conf conf){
	printf("Liste blanche:\n%s\n", conf.listeBlanche);
	printf("Liste noire:\n%s\n", conf.listeNoire);
	Auth_Var_Conf = conf;
}

void load(){
	printf("Chargement de la liste blanche\n");
	loadListe(Auth_Var_Conf.listeBlanche, &Auth_Var_Liste_Blanche);

	printf("Chargement de la liste noire\n");
	loadListe(Auth_Var_Conf.listeNoire, &Auth_Var_Liste_Noire);

	printf("Listes chargées avec succès\n");

}

void loadListe(char* lien, Liste* l){
	printf("Adresse du fichier: %s\n", lien);

	FILE* fd;

	fd = fopen(lien, "r");
	if(fd == NULL){
		perror("Impossible d'ouvrir le fichier\n");
		exit(EXIT_FAILURE);
	}
	char ligne[255];

	while(feof(fd) == 0){
		fscanf(fd,"%s\n",ligne);
		addElt(l, (void*) ligne);
		printf("Element ajouté: %s\n", ligne);
	}

	fclose(fd);

}
