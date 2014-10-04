all: playwave.o 
	g++ -ggdb -Wall `sdl-config --cflags` xdoutils.c xev.c -lSDL_mixer -lxdo playwave.o -lXrandr -lX11 -o xev 
playwave.o:
	g++ -ggdb -Wall -c `sdl-config --cflags` -lSDL_mixer sdl/playwave.c 
xdo.o: xdoutils.c
	g++ -ggdb -Wall -c xdoutils.c -lxdo -o xdo.o
