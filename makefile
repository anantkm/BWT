all: bwtdecode bwtsearch

bwtdecode: bwtdecode.c
	gcc -O3 -o bwtdecode bwtdecode.c

bwtsearch: bwtsearch.c
	gcc -O3 -o bwtsearch bwtsearch.c