all: aplay.o xdo.o
	gcc -Wall xev.c aplay.o xdo.o -lXrandr -lX11 -lasound -lxdo -o xev
aplay.o: aplay.c
	gcc -Wall -c aplay.c -lasound -o aplay.o
xdo.o: xdoutils.c
	gcc -Wall -c xdoutils.c -lX11 -lxdo -o xdo.o
