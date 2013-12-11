#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

int main(){
	printf("Bienvenue dans la partie cache\n");
	char tmp[] = "./tmp";
	initCache(10, tmp);

	Cache_Elt e;
	e = generate("test");
	addEltCache(e);

	closeCache();
	return 0;
}
