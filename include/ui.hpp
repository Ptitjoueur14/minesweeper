#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "board.hpp"

void create_window(Board &board);
void drawAllCells(SDL_Renderer *renderer, Board &board, TTF_Font *font);
void drawGameStatistics(SDL_Renderer *renderer, Board &board, TTF_Font *font);

#endif
