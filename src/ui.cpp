#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include "../include/ui.hpp"

void create_window(Board board)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return;
    }
    
    SDL_Window* window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int window_offset = 40;
    SDL_RenderDrawLine(renderer, window_offset, window_offset, 1920 - window_offset, window_offset);
    SDL_RenderDrawLine(renderer, window_offset, window_offset, window_offset, 1080 - window_offset);
    SDL_RenderDrawLine(renderer, 1920 - window_offset, window_offset, 1920 - window_offset, 1080 - window_offset);
    SDL_RenderDrawLine(renderer, window_offset, 1080 - window_offset, 1920 - window_offset, 1080 - window_offset);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    
    drawAllCells(renderer, board);
    SDL_RenderPresent(renderer);

    SDL_Delay(20000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void drawAllCells(SDL_Renderer *renderer, Board board)
{
    SDL_Rect rect;
    rect.w = 30;
    rect.h = 30;
    for (int i = 0; i < board.width; i++)
    {
        for (int j = 0; j < board.height; j++)
        {
            rect.x = 50 + 50 * i;
            rect.y = 50 + 50 * j;

            Cell cell = board.getCell(i, j);
            if (cell.isMine)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
            }
            else
            {  
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // gray
            }
            SDL_RenderFillRect(renderer, &rect);
            
        }
    }
}
