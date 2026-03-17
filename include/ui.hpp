#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_ttf.h>
#include "board.hpp"

void create_window(Board &board);
void drawAllCells(SDL_Renderer *renderer, Board &board, TTF_Font *font);
void drawGameStatistics(SDL_Renderer *renderer, Board &board, TTF_Font *font);

void clickCell(SDL_Event &event, Board &board);
void revealCell(int x, int y, Board &board);
#endif
