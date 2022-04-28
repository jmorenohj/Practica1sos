
all: indexar interfaz search clean

indexar: indexar.c
	gcc indexar.c -o indexar

map.o: map.c map.h
	gcc -c map.c


interfaz: interfaz.c 
	gcc interfaz.c -o interfaz 


search.o: search.c map.h 
	gcc -c search.c

search: search.o map.o 
	gcc -o search search.o map.o 



clean: indexed.txt aux.txt
	rm indexed.txt aux.txt


