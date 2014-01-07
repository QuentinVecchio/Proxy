#include "auth.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include <string.h>


void init(Auth_Conf conf){
	printf("Liste blanche:\n%s\n", conf.listeBlanche);
	printf("Liste noire:\n%s\n", conf.listeNoire);
	printf("Liste règles:\n%s\n", conf.listeRegle);
	printf("Liste extensions:\n%s\n", conf.listeExt);


	Auth_Var_Conf = conf;
	initListe(&Auth_Var_Liste_Blanche);
	initListe(&Auth_Var_Liste_Noire);
	initListe(&Auth_Var_Liste_Regle);
	initListe(&Auth_Var_Liste_Ext);
}

int load(){
	printf("Chargement de la liste blanche\n");
	int successA = loadListe(Auth_Var_Conf.listeBlanche, &Auth_Var_Liste_Blanche);

	printf("Chargement de la liste noire\n");
	int successB = loadListe(Auth_Var_Conf.listeNoire, &Auth_Var_Liste_Noire);

	printf("Chargement de la liste des règles\n");
	int successC = loadRules(Auth_Var_Conf.listeRegle, &Auth_Var_Liste_Regle);

	printf("Chargement de la liste des extensions\n");
	int successD = loadListe(Auth_Var_Conf.listeExt, &Auth_Var_Liste_Ext);

	if(!successA && !successA && !successC &&!successD){
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
		Auth_Regle *a_R = malloc(sizeof(Auth_Regle));

		char *tmp = malloc(sizeof(char)*2046);
		char *lien = malloc(sizeof(char)*500);
		char *ip = malloc(sizeof(char)*20);
		char *valeur = malloc(sizeof(char)*5);

		fscanf(fd,"%s\n",tmp);
		lien = strsep(&tmp, ",");
		ip = strsep(&tmp, ",");
		valeur = strsep(&tmp, ",");

		a_R->link = lien;
		a_R->address = ip;
		a_R->status = atoi(valeur);
		addElt(l, (void*) a_R);
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

	int res = recherche(&a_S->listeRecherche[index],(void*) a_S->params[index], a_S->fonctionCmp[index]);
	a_S->estDansListe[index] = res;
	pthread_exit(NULL);
}

int cmp_lien(void* valeur, void* elt){
	char* a = *((char**) &elt);
	char* b = (char*) valeur;
	return !strcmp(a,b);
}

int cmp_regle(void* valeur, void* elt){
	Auth_Regle *v = (Auth_Regle*) valeur;
	Auth_Regle *e = (Auth_Regle*) elt;

	if(strcmp(v->link, e->link)){
		return 0;
	}

	if(strcmp(v->address, e->address)) return 0;

	return e->status;

}


int isAuthorized(char* lien, char* address, char* ext){
	printf("Recherche du lien: %s\n", lien);

	Auth_Search a_S;
	a_S.index = 0;
	pthread_mutex_init(&a_S.m_index, NULL);

	a_S.estDansListe[0] = 0;
	a_S.fonctionCmp[0]= &cmp_lien;
	a_S.params[0] = (void*) lien;


	a_S.estDansListe[1] = 0;
	a_S.fonctionCmp[1]= &cmp_lien;
	a_S.params[1] = (void*) lien;

	Auth_Regle a_R;
	a_R.link = strdup(lien);
	a_R.address = address;

	a_S.estDansListe[2] = 0;
	a_S.fonctionCmp[2]= &cmp_regle;
	a_S.params[2] = (void*) &a_R;

	a_S.estDansListe[3] = 0;
	a_S.fonctionCmp[3]= &cmp_lien;
	a_S.params[3] = (void*) ext;


	a_S.listeRecherche[0] = Auth_Var_Liste_Blanche;
	a_S.listeRecherche[1] = Auth_Var_Liste_Noire;
	a_S.listeRecherche[2] = Auth_Var_Liste_Regle;
	a_S.listeRecherche[3] = Auth_Var_Liste_Ext;
	pthread_t t[NB_THREAD];
	
	int i;
	pthread_create(&t[0], NULL, thread_search, (void*) &a_S);
	pthread_create(&t[1], NULL, thread_search, (void*) &a_S);
	pthread_create(&t[2], NULL, thread_search, (void*) &a_S);
	pthread_create(&t[3], NULL, thread_search, (void*) &a_S);


	for(i=0; i < NB_THREAD; i++){
		pthread_join(t[i], NULL);
	}

	if(a_S.estDansListe[2] == 1 || a_S.estDansListe[0] == 1 || a_S.estDansListe[1] == 0) return 1;
	if(a_S.estDansListe[2] == -1) return 2;
	if(a_S.estDansListe[3] == 1) return 3;

}




