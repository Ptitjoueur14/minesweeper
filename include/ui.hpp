#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
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
};

void create_window();
void drawAllCells();
void drawGameStatistics();

void clickCell();
void revealCell(int x, int y, Board &board);
#endif
