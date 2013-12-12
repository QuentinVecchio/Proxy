#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

int main(){
	printf("Bienvenue dans la partie cache\n");
	char tmp[] = "./tmp";
	initCache(10, tmp);

	Cache_Elt* e;
	int i = 2;
	e = generate("test");
	addEltCache(*e);

	Cache_Elt* res = isInCache("test");

	if(res == NULL){
		printf("Element non trouvé\n");
	}else{
		printf("Element trouvé !!!\n");
	}
	closeCache();
	pause();
	return 0;
}
