all: playwave.o 
	g++ -ggdb -Wall -std=c++11 `curl-config --cflags` `curl-config --libs` `sdl-config --cflags` xev.c `sdl-config --libs` -lSDL_mixer -lxdo playwave.o -lXrandr -lX11 -o xev 
playwave.o:
	g++ -ggdb -Wall -c -std=c++11 `sdl-config --cflags` `sdl-config --libs` -lSDL_mixer sdl/playwave.c 
