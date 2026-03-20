#ifndef DRAW_HPP
#define DRAW_HPP

#include <iostream>
#include <SDL2/SDL.h>

class Draw
{
    public:
        // Called at the start of the game to draw borders
        static void drawStaticUI();

        // Called when board state changes (on click)
        static void redrawBoardUI();

        static void drawTextInCell(const std::string &text, SDL_Rect cellRect, SDL_Color color);

        static void drawText(const std::string &text, SDL_Rect textRect, SDL_Color color);

        static void drawAllCells();

        static void drawNumber(int cellX, int cellY);

        static void drawGameStatistics();

        static void drawGameInfo();

        static void drawGameFinishInfo();

        // Draws a colored square in the cell (cellX, cellY)
        static void drawSquare(int cellX, int cellY, SDL_Color color);

        static void drawTextureInCell(int cellX, int cellY, SDL_Texture *texture);
};

#endif
