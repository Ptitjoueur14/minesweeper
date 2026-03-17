#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include "board.hpp"

void create_window(Board board);
void drawAllCells(SDL_Renderer *renderer, Board board);

#endif
