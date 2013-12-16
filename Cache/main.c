#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

int main(){
	printf("Bienvenue dans la partie cache\n");
	char tmp[] = "./tmp";
	initCache(10, tmp);

	Cache_Elt* e;
	int i = 2;
	/**
	*	Ajout d'élément aléatoire
	*/
	while(1){
		printf("Ajout\n");
		int var = rand()%150+1;
		char* vart = malloc(sizeof(char)*1024);
		sprintf(vart,"%d", var);
		e = generate(vart);
		addEltCache(e);
		sleep(rand()%5+1);
	}
	/*if(res == NULL){
		printf("Element non trouvé\n");
	}else{
		printf("Element trouvé !!!\n");
	}*/
	//closeCache();
	pause();
	return 0;
}
