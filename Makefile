all:
	g++ src/board.cpp src/game.cpp src/ui.cpp -g -O0 -o game \
	-I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_ttf

clean:
	rm -f game
