all:
	g++ src/board.cpp src/game.cpp src/timer.cpp src/draw.cpp src/ui.cpp -fsanitize=address -g -o game \
	-I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_ttf -lSDL2_image
	./game

clean:
	rm -f game
