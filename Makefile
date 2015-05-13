all: playwave.o sendmail.o
	g++ -ggdb -Wall -std=c++11 `curl-config --cflags` `curl-config --libs` `sdl-config --cflags` `sdl-config --libs` -lSDL_mixer -lxdo playwave.o -lXrandr -lX11 -letpan sendmail.c xev.c -o xev 
playwave.o:
	g++ -ggdb -Wall -c -std=c++11 `sdl-config --cflags` `sdl-config --libs` -lSDL_mixer sdl/playwave.c 
sendmail.o:
	gcc -ggdb -Wall -c -letpan sendmail.c 

