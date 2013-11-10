#include "Auth.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include <string.h>
void init(Auth_Conf conf){
	printf("Liste blanche:\n%s\n", conf.listeBlanche);
	printf("Liste noire:\n%s\n", conf.listeNoire);
	Auth_Var_Conf = conf;
	initListe(&Auth_Var_Liste_Blanche);
	initListe(&Auth_Var_Liste_Noire);
}

void t(void* a){
	char* tmp = *((char**) &a);
	printf("Valeur : %s \n", tmp);
}


void load(){
	printf("Chargement de la liste blanche\n");
	loadListe(Auth_Var_Conf.listeBlanche, &Auth_Var_Liste_Blanche);

	printf("Chargement de la liste noire\n");
	loadListe(Auth_Var_Conf.listeNoire, &Auth_Var_Liste_Noire);

	printf("Listes chargées avec succès\n");
	
}




void* thread_search(void* arg){
	Auth_Search* a_S = (Auth_Search*) arg;
	int index;
	pthread_mutex_lock(&a_S->m_index);
	index = a_S->index;
	a_S->index++;
	pthread_mutex_unlock(&a_S->m_index);

	printf("Index: %d, Valeur: %s\n",index, a_S->lien);
	printf("Index: %d, Recherche:\n", index);
	int res = recherche(&a_S->listeRecherche[index],(void*) a_S->lien, a_S->fonctionCmp[index]);
	if(res){
		printf("Index: %d, trouvé!\n", index);
	}else{
		printf("Index: %d, non trouvé :(\n",index);
	}
}

int cmp_lien(void* valeur, void* elt){
	char* a = *((char**) &elt);
	char* b = (char*) valeur;
	printf("Comparaison de %s avec %s\n",a,b);
	return !strcmp(a,b);
}

int search(char* lien){
	printf("Recherche du lien: %s\n", lien);
	Auth_Search a_S;
	a_S.lien = lien;
	a_S.index = 0;
	pthread_mutex_init(&a_S.m_index, NULL);

	int i;
	for(i=0; i < NB_THREAD; i++){
		a_S.estDansListe[i] = 0;
		a_S.fonctionCmp[i]= &cmp_lien;
	}

	a_S.listeRecherche[0] = Auth_Var_Liste_Blanche;
	a_S.listeRecherche[1] = Auth_Var_Liste_Noire;
	pthread_t t[NB_THREAD];
	
	for(i=0; i < NB_THREAD; i++){
		pthread_create(&t[i], NULL, thread_search, (void*) &a_S);
	}

	for(i=0; i < NB_THREAD; i++){
		pthread_join(t[i], NULL);
	}
	return 0;
}



void loadListe(char* lien, Liste* l){
	printf("Adresse du fichier: %s\n", lien);

	FILE* fd;

	fd = fopen(lien, "r");
	if(fd == NULL){
		perror("Impossible d'ouvrir le fichier\n");
		exit(EXIT_FAILURE);
	}

	while(feof(fd) == 0){
		char ligne[255];

		fscanf(fd,"%s\n",ligne);
		addElt(l, (void*) strdup(ligne));
	}
	printf("Affiche\n");
	affiche(l, t);
	
	fclose(fd);

}
