all: aplay.o 
	gcc xev.c aplay.o -lXrandr -lX11 -lasound -o xev
aplay.o: aplay.c
	gcc -c aplay.c -lasound -o aplay.o
