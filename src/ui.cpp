#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "../include/ui.hpp"

#define WINDOW_WIDTH 1850
#define WINDOW_HEIGHT 1020

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

    SDL_Rect usableBounds;
    if (SDL_GetDisplayUsableBounds(0, &usableBounds) < 0)
    {
        std::cerr << "SDL_GetDisplayUsableBounds failed: " << SDL_GetError() << std::endl;
        usableBounds.x = 0;
        usableBounds.y = 0;
        usableBounds.w = 1920;
        usableBounds.h = 1080;
    }

    int posX = usableBounds.x + usableBounds.w - WINDOW_WIDTH;
    int posY = usableBounds.y;
    SDL_Window* window = SDL_CreateWindow("Minesweeper", posX, posY, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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

    bool isRunning = true;
    SDL_Event event;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                std::cout << "click" << std::endl;
                clickCell(event, board);
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        int window_offset = 40;
        SDL_RenderDrawLine(renderer, window_offset, window_offset, WINDOW_WIDTH - window_offset, window_offset);
        SDL_RenderDrawLine(renderer, window_offset, window_offset, window_offset, WINDOW_HEIGHT - window_offset);
        SDL_RenderDrawLine(renderer, WINDOW_WIDTH - window_offset, window_offset, WINDOW_WIDTH - window_offset, WINDOW_HEIGHT - window_offset);
        SDL_RenderDrawLine(renderer, window_offset, WINDOW_HEIGHT - window_offset, WINDOW_WIDTH - window_offset, WINDOW_HEIGHT - window_offset);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        
        drawAllCells(renderer, board, font);
        drawGameStatistics(renderer, board, font);
        SDL_RenderPresent(renderer);
    }
    
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
    int window_offset = 50;
    
    SDL_Rect cellRect;
    int cellSizeX = (WINDOW_WIDTH - window_offset * 2) / board.width;
    int cellSizeY = (WINDOW_HEIGHT - window_offset * 2) / board.height;

    int minCellSize;
    if (cellSizeX < cellSizeY)
    {
        minCellSize = cellSizeX;
    }
    else
    {
        minCellSize = cellSizeY;
    }

    int textSize = minCellSize / 2;
    TTF_SetFontSize(font, textSize);
    
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
                std::vector<SDL_Color> textColors = {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {255, 0, 255},
                                                    {245, 159, 22}, {22, 230, 245}, {200, 200, 200}, {150, 150, 150}};
                SDL_Color textColor = textColors.at(cell.adjacentMines - 1);                
                std::string text = std::to_string(cell.adjacentMines);
                drawText(renderer, font, text, cellRect, textColor);
            }
        }
    }
}

void drawGameStatistics(SDL_Renderer *renderer, Board &board, TTF_Font *font)
{
    SDL_Rect statsRect;
    statsRect.x = 160;
    statsRect.y = 10;
    statsRect.w = 300;
    statsRect.h = 30;

    SDL_Color statsTextColor = {255, 255, 0};
    int fontSize = 15;
    TTF_SetFontSize(font, fontSize);

    char statsBuffer[100];
    snprintf(statsBuffer, sizeof(statsBuffer), "Board size: %ix%i/%i, Mine density: %.2f%s", board.width, board.height, board.minesCount, (float) board.minesCount / board.totalCells * 100, "%");
    std::string statsText = statsBuffer;
    drawText(renderer, font, statsText, statsRect, statsTextColor);
}

void clickCell(SDL_Event &event, Board &board)
{
    if (event.button.button == SDL_BUTTON_LEFT)
    {
        int x = event.button.x;
        int y = event.button.y;
        if (x < 50 || x > WINDOW_WIDTH - 50 || y < 50 || y > WINDOW_HEIGHT)
        {
            return;
        }
        
        int mouseX = x - 50;
        int mouseY = y - 50;
        int cellSizeX = (WINDOW_WIDTH - 50 * 2) / board.width;
        int cellSizeY = (WINDOW_HEIGHT - 50 * 2) / board.height;
        int minCellSize;
        if (cellSizeX < cellSizeY)
        {
            minCellSize = cellSizeX;
        }
        else
        {
            minCellSize = cellSizeY;
        }
        
        int cellX = mouseX / minCellSize;
        int cellY = mouseY / minCellSize;
        std::cout << "Clicked on cell " << cellX << "; " << cellY << std::endl;

        Cell cell = board.getCell(cellX, cellY);
        if (cell.isMine)
        {
            std::cout << "You clicked on a mine and lost !" << std::endl;
            return;
        }

        cell.isRevealed = true;
        revealCell(cellX, cellY, board);
    }
}

void revealCell(int x, int y, Board &board)
{
    Cell cell = board.getCell(x, y);
}
