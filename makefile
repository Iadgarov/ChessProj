INCLUDES = -I/usr/local/stow/SDL_image-1.2.12/lib/SDL_image-1.2.12/include/SDL

LFLAGS = -L/usr/local/stow/SDL_image-1.2.12/lib/SDL_image-1.2.12/lib

LIBS = -lSDL_image

all: chessprog

clean:
	-rm Chess.o Utilities.o chessprog

chessprog: Chess.o Utilities.o
	gcc  -o chessprog Chess.o Utilities.o -lm -ansi -pedantic-errors -g `sdl-config --libs` $(LFLAGS) $(LIBS)

Chess.o: Chess.c
	gcc  -ansi -pedantic-errors -c -Wall -g -lm Chess.c `sdl-config --cflags`  $(INCLUDES)

Utilities.o: Utilities.c
	gcc  -ansi -pedantic-errors -c -Wall -g -lm Utilities.c `sdl-config --cflags`  $(INCLUDES)
