all: aplay.o 
	gcc -Wall xev.c aplay.o -lXrandr -lX11 -lasound -o xev
aplay.o: aplay.c
	gcc -Wall -c aplay.c -lasound -o aplay.o
