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

SDL_Window *GameUI::window = nullptr;
SDL_Renderer *GameUI::renderer = nullptr;
TTF_Font *GameUI::font = nullptr;
SDL_Event GameUI::event;
Board *GameUI::board = nullptr;

#define WINDOW_WIDTH 1850
#define WINDOW_HEIGHT 1020

void create_window()
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
    GameUI::window = SDL_CreateWindow("Minesweeper", posX, posY, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    GameUI::renderer = SDL_CreateRenderer(GameUI::window, -1, SDL_RENDERER_ACCELERATED);

    GameUI::font = TTF_OpenFont("assets/minesweeper-font/mine-sweeper.otf", 24);
    if (!GameUI::font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(GameUI::renderer);
        SDL_DestroyWindow(GameUI::window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    bool isRunning = true;

    while (isRunning)
    {
        while (SDL_PollEvent(&GameUI::event))
        {
            if (GameUI::event.type == SDL_QUIT)
            {
                isRunning = false;
            }

            if (GameUI::event.type == SDL_MOUSEBUTTONDOWN)
            {
                std::cout << "click" << std::endl;
                clickCell();
            }
        }
        
        SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255);
        SDL_RenderClear(GameUI::renderer);
        SDL_SetRenderDrawColor(GameUI::renderer, 255, 255, 255, 255);

        int window_offset = 40;
        SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, WINDOW_WIDTH - window_offset, window_offset);
        SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, window_offset, WINDOW_HEIGHT - window_offset);
        SDL_RenderDrawLine(GameUI::renderer, WINDOW_WIDTH - window_offset, window_offset, WINDOW_WIDTH - window_offset, WINDOW_HEIGHT - window_offset);
        SDL_RenderDrawLine(GameUI::renderer, window_offset, WINDOW_HEIGHT - window_offset, WINDOW_WIDTH - window_offset, WINDOW_HEIGHT - window_offset);

        SDL_SetRenderDrawColor(GameUI::renderer, 100, 100, 100, 255);
        
        drawAllCells();
        drawGameStatistics();
        SDL_RenderPresent(GameUI::renderer);
    }
    
    TTF_CloseFont(GameUI::font);

    SDL_DestroyRenderer(GameUI::renderer);
    SDL_DestroyWindow(GameUI::window);

    TTF_Quit();
    SDL_Quit();
}

void drawText(const std::string &text, SDL_Rect cellRect, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(GameUI::font, text.c_str(), color);
    if (!surface)
    {
        std::cerr << "TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(GameUI::renderer, surface);
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

    SDL_RenderCopy(GameUI::renderer, texture, nullptr, &textRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void drawAllCells()
{
    int window_offset = 50;
    
    SDL_Rect cellRect;
    int cellSizeX = (WINDOW_WIDTH - window_offset * 2) / GameUI::board->width;
    int cellSizeY = (WINDOW_HEIGHT - window_offset * 2) / GameUI::board->height;

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
    TTF_SetFontSize(GameUI::font, textSize);
    
    int cellSize = minCellSize;
    int padding = 2; // space between each cell
    cellRect.w = cellSize - padding;
    cellRect.h = cellSize - padding;
    
    for (int i = 0; i < GameUI::board->width; i++)
    {
        for (int j = 0; j < GameUI::board->height; j++)
        {
            cellRect.x = 50 + cellSize * i;
            cellRect.y = 50 + cellSize * j;

            Cell cell = GameUI::board->getCell(i, j);
            if (cell.isMine)
            {
                SDL_SetRenderDrawColor(GameUI::renderer, 255, 0, 0, 255); // red
            }
            else
            {  
                SDL_SetRenderDrawColor(GameUI::renderer, 120, 120, 120, 255); // gray
            }

            // Draw cell first
            SDL_RenderFillRect(GameUI::renderer, &cellRect); 
                
            // Draw adjacency number if not mine and number > 0
            if (!cell.isMine && cell.adjacentMines > 0)
            {
                std::vector<SDL_Color> textColors = {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {255, 0, 255},
                                                    {245, 159, 22}, {22, 230, 245}, {200, 200, 200}, {150, 150, 150}};
                SDL_Color textColor = textColors.at(cell.adjacentMines - 1);                
                std::string text = std::to_string(cell.adjacentMines);
                drawText(text, cellRect, textColor);
            }
        }
    }
}

void drawGameStatistics()
{
    SDL_Rect statsRect;
    statsRect.x = 160;
    statsRect.y = 10;
    statsRect.w = 300;
    statsRect.h = 30;

    SDL_Color statsTextColor = {255, 255, 0};
    int fontSize = 15;
    TTF_SetFontSize(GameUI::font, fontSize);

    char statsBuffer[100];
    snprintf(statsBuffer, sizeof(statsBuffer), "Board size: %ix%i/%i, Mine density: %.2f%s",GameUI::board->width, GameUI::board->height, GameUI::board->minesCount, (float) GameUI::board->minesCount / GameUI::board->totalCells * 100, "%");
    std::string statsText = statsBuffer;
    drawText(statsText, statsRect, statsTextColor);
}

void clickCell()
{
    if (GameUI::event.button.button == SDL_BUTTON_LEFT)
    {
        int x = GameUI::event.button.x;
        int y = GameUI::event.button.y;
        if (x < 50 || x > WINDOW_WIDTH - 50 || y < 50 || y > WINDOW_HEIGHT)
        {
            return;
        }
        
        int mouseX = x - 50;
        int mouseY = y - 50;
        int cellSizeX = (WINDOW_WIDTH - 50 * 2) / GameUI::board->width;
        int cellSizeY = (WINDOW_HEIGHT - 50 * 2) / GameUI::board->height;
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

        Cell cell = GameUI::board->getCell(cellX, cellY);
        if (cell.isMine)
        {
            std::cout << "You clicked on a mine and lost !" << std::endl;
            return;
        }

        cell.isRevealed = true;
        revealCell(cellX, cellY, *GameUI::board);
    }
}

void revealCell(int x, int y, Board &board)
{
    Cell cell = board.getCell(x, y);
}
