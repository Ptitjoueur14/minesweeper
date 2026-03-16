all:
	g++ src/board.cpp src/game.cpp -g -O0 -o game

clean:
	rm game
