#.PHONY: clean, reset

#.SUFFIXES:

all : main.o proxy.o auth.o cache.o listeGenerique.o 
	gcc -O3 main.o proxy.o auth.o cache.o listeGenerique.o -o ProxyMQ  -lpthread

main.o : main.c Proxy/proxy.h Auth/auth.h ListeGenerique/listeGenerique.h Cache/cache.h
	gcc -c -O3 main.c -o main.o

proxy.o : Proxy/proxy.c Auth/auth.h ListeGenerique/listeGenerique.h Cache/cache.h
	gcc -c -O3 Proxy/proxy.c -o proxy.o -lpthread

auth.o : Auth/auth.c ListeGenerique/listeGenerique.h
	gcc -c -O3 Auth/auth.c -o auth.o -lpthread

cache.o : Cache/cache.c ListeGenerique/listeGenerique.h
	gcc -c -O3 Cache/cache.c -o cache.o -lpthread

listeGenerique.o : ListeGenerique/listeGenerique.c
	gcc -c -O3 ListeGenerique/listeGenerique.c -o listeGenerique.o

#effacer les fichers '*.o'
clean :
	rm -rf *.o

#efface les fichier '*.o' plus l'executable 'Sudoku'
reset : clean
	rm -rf ProxyMQ
