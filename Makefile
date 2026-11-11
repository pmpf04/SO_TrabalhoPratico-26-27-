all: central tarifador

central: central.c central.h util.h
	gcc -o central central.c

tarifador: tarifador.c
	gcc -o tarifador tarifador.c

clean:
	rm -f *.o central tarifador

