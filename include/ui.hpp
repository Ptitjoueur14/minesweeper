#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>

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
        static int leftClicks;
        static int rightClicks;
};

void create_window();
void drawStaticUI();
void redrawBoardUI();
void drawGameInfo();

void drawSquare(int cellX, int cellY, SDL_Color color);
void drawNumber(int cellX, int cellY);

void drawAllCells();
void drawGameStatistics();
void drawTextureInCell(int cellX, int cellY, SDL_Texture *texture);

void clickCell();
void revealCell(int cellX, int cellY);

void flagCell();
void chordCell(int cellX, int cellY);

void checkForGameFinish();
void finishGame(bool isWon);

void resetBoard();

#endif
