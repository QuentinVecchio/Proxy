#include "Auth.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include <string.h>


void init(Auth_Conf conf){
	printf("Liste blanche:\n%s\n", conf.listeBlanche);
	printf("Liste noire:\n%s\n", conf.listeNoire);
	printf("Liste règles:\n%s\n", conf.listeRegle);

	Auth_Var_Conf = conf;
	initListe(&Auth_Var_Liste_Blanche);
	initListe(&Auth_Var_Liste_Noire);
	initListe(&Auth_Var_Liste_Regle);
}

int load(){
	printf("Chargement de la liste blanche\n");
	int successA = loadListe(Auth_Var_Conf.listeBlanche, &Auth_Var_Liste_Blanche);

	printf("Chargement de la liste noire\n");
	int successB = loadListe(Auth_Var_Conf.listeNoire, &Auth_Var_Liste_Noire);

	printf("Chargement de la liste des règles\n");
	int successC = loadRules(Auth_Var_Conf.listeRegle, &Auth_Var_Liste_Regle);

	if(!successA && !successA && !successC){
		printf("Listes chargées avec succès\n");
		return 0;
	}else{
		printf("Erreur lors du chargement d'une (ou plusieurs) liste(s)\n");
		return 1;
	}
}

int loadListe(char* lien, Liste* l){
	printf("Adresse du fichier: %s\n", lien);

	FILE* fd;

	fd = fopen(lien, "r");
	if(fd == NULL){
		perror("Impossible d'ouvrir le fichier\n");
		return 1;
	}

	while(feof(fd) == 0){
		char ligne[255];

		fscanf(fd,"%s\n",ligne);
		addElt(l, (void*) strdup(ligne));
	}

	fclose(fd);
	return 0;
}


int loadRules(char* lien, Liste* l){
	printf("Adresse du fichier: %s\n", lien);

	FILE* fd;

	fd = fopen(lien, "r");
	if(fd == NULL){
		perror("Impossible d'ouvrir le fichier\n");
		return 1;
	}

	while(feof(fd) == 0){
		Auth_Regle a_R;
		printf("Boucle\n");
		char tmp[2046];
		fscanf(fd,"%s\n",tmp);
		//a_R.link = *strsep(&tmp, ",");

		printf("valeur: %s\n", tmp);
		addElt(l, (void*) &a_R);
	}

	fclose(fd);
	return 0;
}


void* thread_search(void* arg){
	Auth_Search* a_S = (Auth_Search*) arg;
	int index;
	pthread_mutex_lock(&a_S->m_index);
	index = a_S->index;
	a_S->index++;
	pthread_mutex_unlock(&a_S->m_index);

	int res = recherche(&a_S->listeRecherche[index],(void*) a_S->lien, a_S->fonctionCmp[index]);
	a_S->estDansListe[index] = res;
	pthread_exit(NULL);
}

int cmp_lien(void* valeur, void* elt){
	char* a = *((char**) &elt);
	char* b = (char*) valeur;
	return !strcmp(a,b);
}

int isAuthorized(char* lien){
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
	return a_S.estDansListe[0] || a_S.estDansListe[1];
}




