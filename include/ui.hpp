#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "board.hpp"

class GameUI
{
    public:
        static TTF_Font *font;
        static SDL_Event event;
        static SDL_Window *window;
        static SDL_Renderer *renderer;
        static Board *board;
        static int cellSize;
        static bool isGameFinished;
        static bool isGameWon;

        static SDL_Texture *flagTexture;
        static SDL_Texture *mineTexture;

        static int nbClicks;
};

void create_window();
void drawStaticUI();
void redrawBoardUI();
void drawSquare(int cellX, int cellY, SDL_Color color);
void drawAllCells();
void drawGameStatistics();
void drawTextureInCell(int cellX, int cellY, SDL_Texture *texture);

void clickCell();
void revealCell(int x, int y);

void flagCell();
void chordCell(int x, int y);

void checkForGameFinish();
void finishGame();

void resetBoard();

#endif
