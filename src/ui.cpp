#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "../include/ui.hpp"

void create_window(Board &board)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return;
    }

    if (TTF_Init() < 0)
    {
        std::cerr << "TTF Init Failed: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Window* window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int window_offset = 40;
    SDL_RenderDrawLine(renderer, window_offset, window_offset, 1920 - window_offset, window_offset);
    SDL_RenderDrawLine(renderer, window_offset, window_offset, window_offset, 1080 - window_offset);
    SDL_RenderDrawLine(renderer, 1920 - window_offset, window_offset, 1920 - window_offset, 1080 - window_offset);
    SDL_RenderDrawLine(renderer, window_offset, 1080 - window_offset, 1920 - window_offset, 1080 - window_offset);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    TTF_Font *font = TTF_OpenFont("assets/minesweeper-font/mine-sweeper.otf", 24);
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }
    
    drawAllCells(renderer, board, font);
    SDL_RenderPresent(renderer);

    SDL_Delay(20000);

    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Rect cellRect, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface)
    {
        std::cerr << "TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect textRect;
    textRect.w = surface->w;
    textRect.h = surface->h;
    textRect.x = cellRect.x + (cellRect.w - textRect.w) / 2;
    textRect.y = cellRect.y + (cellRect.h - textRect.h) / 2;

    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void drawAllCells(SDL_Renderer *renderer, Board &board, TTF_Font *font)
{    
    SDL_Rect cellRect;
    int cellSizeX = 1920 / board.width;
    int cellSizeY = 1080 / board.height;
    int minCellSize;
    if (cellSizeX < cellSizeY)
    {
        minCellSize = cellSizeX;
    }
    else
    {
        minCellSize = cellSizeY;
    }
    int cellSize = minCellSize;
    int padding = 2; // space between each cell
    cellRect.w = cellSize - padding;
    cellRect.h = cellSize - padding;
    
    for (int i = 0; i < board.width; i++)
    {
        for (int j = 0; j < board.height; j++)
        {
            cellRect.x = 50 + cellSize * i;
            cellRect.y = 50 + cellSize * j;

            Cell cell = board.getCell(i, j);
            if (cell.isMine)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
            }
            else
            {  
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255); // gray
            }

            // Draw cell first
            SDL_RenderFillRect(renderer, &cellRect); 
                
            // Draw adjacency number if not mine and number > 0
            if (!cell.isMine && cell.adjacentMines > 0)
            {
                SDL_Color text_color;
                switch (cell.adjacentMines)
                {
                    case 1:
                        text_color = {0, 0, 255};
                        break;
                    case 2:
                        text_color = {0, 255, 0};
                        break;
                    case 3:
                        text_color = {255, 0, 0};
                        break;
                    default:
                        text_color = {200, 50, 0};
                        break;
                }
                
                std::string text = std::to_string(cell.adjacentMines);
                drawText(renderer, font, text, cellRect, text_color);
            }
        }
    }
}
