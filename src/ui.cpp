#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hidapi.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "../include/ui.hpp"

SDL_Window *GameUI::window = nullptr;
SDL_Renderer *GameUI::renderer = nullptr;
TTF_Font *GameUI::font = nullptr;
SDL_Event GameUI::event;
Board *GameUI::board = nullptr;
int GameUI::cellSize = 40;
bool GameUI::isGameFinished = false;
bool GameUI::isGameWon = false;
SDL_Texture *GameUI::flagTexture = nullptr;
SDL_Texture *GameUI::mineTexture = nullptr;
int GameUI::nbClicks = 0;

const char revealCellKey = 'a';

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

    if (IMG_Init(0) < 0)
    {
        std::cerr << "IMG Init Failed: " << IMG_GetError() << std::endl;
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
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }

    GameUI::flagTexture = IMG_LoadTexture(GameUI::renderer, "assets/flag.png");
    if (!GameUI::flagTexture)
    {
        std::cout << "Failed to load flag texture: " << IMG_GetError << std::endl;
        return;
    }
    GameUI::mineTexture = IMG_LoadTexture(GameUI::renderer, "assets/mine.png");
    if (!GameUI::mineTexture)
    {
        std::cout << "Failed to load mine texture: " << IMG_GetError << std::endl;
        return;
    }
    
    drawStaticUI();    
    drawAllCells();
    drawGameStatistics();

    SDL_RenderPresent(GameUI::renderer);
    
    bool isRunning = true;

    while (isRunning)
    {
        while (SDL_PollEvent(&GameUI::event))
        {
            // Close window with "X" button
            if (GameUI::event.type == SDL_QUIT)
            {
                isRunning = false;
            }

            // Open cell with left click or "A"
            if ((GameUI::event.type == SDL_MOUSEBUTTONDOWN && GameUI::event.button.button == SDL_BUTTON_LEFT) ||
                (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_a))
            {
                clickCell();
                redrawBoardUI();
                SDL_RenderPresent(GameUI::renderer);
            }
            
            // Flag cell with right click or "Q"
            if ((GameUI::event.type == SDL_MOUSEBUTTONDOWN && GameUI::event.button.button == SDL_BUTTON_RIGHT) ||
                (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_q))
            {
                flagCell();
                redrawBoardUI();
                SDL_RenderPresent(GameUI::renderer);
            }

            // Reset board
            if (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_SPACE)
            {
                resetBoard();
                redrawBoardUI();
                SDL_RenderPresent(GameUI::renderer);
            }
        }
    }

    if (GameUI::flagTexture)
    {
        SDL_DestroyTexture(GameUI::flagTexture);
    }
    if (GameUI::mineTexture)
    {
        SDL_DestroyTexture(GameUI::mineTexture);
    }
    
    TTF_CloseFont(GameUI::font);

    SDL_DestroyRenderer(GameUI::renderer);
    SDL_DestroyWindow(GameUI::window);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

// Called at the start of the game to draw borders
void drawStaticUI()
{
    SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255);
    SDL_RenderClear(GameUI::renderer);

    SDL_SetRenderDrawColor(GameUI::renderer, 255, 255, 255, 255);

    int window_offset = 40;
    SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, WINDOW_WIDTH - window_offset, window_offset);
    SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, window_offset, WINDOW_HEIGHT - window_offset);
    SDL_RenderDrawLine(GameUI::renderer, WINDOW_WIDTH - window_offset, window_offset, WINDOW_WIDTH - window_offset, WINDOW_HEIGHT - window_offset);
    SDL_RenderDrawLine(GameUI::renderer, window_offset, WINDOW_HEIGHT - window_offset, WINDOW_WIDTH - window_offset, WINDOW_HEIGHT - window_offset);
}

// Called when board state changes (on click)
void redrawBoardUI()
{
    SDL_Rect board_rect;
    board_rect.x = 50;
    board_rect.y = 50;
    board_rect.w = GameUI::cellSize * GameUI::board->width;
    board_rect.h = GameUI::cellSize * GameUI::board->height;

    // Clear only board area
    SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(GameUI::renderer, &board_rect);

    drawAllCells();
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

    int cellSizeX = (WINDOW_WIDTH - window_offset * 2) / GameUI::board->width;
    int cellSizeY = (WINDOW_HEIGHT - window_offset * 2) / GameUI::board->height;

    int minCellSize = (cellSizeX < cellSizeY) ? cellSizeX : cellSizeY;

    GameUI::cellSize = minCellSize;

    int textSize = minCellSize / 2;
    TTF_SetFontSize(GameUI::font, textSize);

    for (int i = 0; i < GameUI::board->width; i++)
    {
        for (int j = 0; j < GameUI::board->height; j++)
        {
            Cell &cell = GameUI::board->getCell(i, j);

            // Hidden cell
            if (!GameUI::isGameFinished && !cell.isRevealed && !cell.isFlagged)
            {
                SDL_Color hiddenColor = {120, 120, 120, 255};
                drawSquare(i, j, hiddenColor);
                continue;
            }

            // Revealed mine
            if (cell.isMine && cell.isRevealed)
            {
                SDL_Color mineColor = {255, 0, 0, 255};
                drawSquare(i, j, mineColor);
                drawTextureInCell(i, j, GameUI::mineTexture);
                continue;
            }

            // Flag
            if (!GameUI::isGameFinished && cell.isFlagged)
            {
                SDL_Color hiddenColor = {120, 120, 120, 255};
                drawSquare(i, j, hiddenColor);
                drawTextureInCell(i, j, GameUI::flagTexture);
                continue;
            }

            // Unplaced mine at game win (place flag)
            if (GameUI::isGameFinished && GameUI::isGameWon && cell.isMine && !cell.isRevealed)
            {
                SDL_Color hiddenColor = {120, 120, 120, 255};
                drawSquare(i, j, hiddenColor);
                drawTextureInCell(i, j, GameUI::flagTexture);
                continue;
            }
            
            // Unplaced mine at game loss (place mine)
            if (GameUI::isGameFinished && !GameUI::isGameWon && cell.isMine && !cell.isRevealed)
            {
                SDL_Color hiddenColor = {120, 120, 120, 255};
                drawSquare(i, j, hiddenColor);
                drawTextureInCell(i, j, GameUI::mineTexture);
                continue;
            }

            SDL_Color revealedColor = {180, 180, 180, 255};
            drawSquare(i, j, revealedColor);

            if (cell.adjacentMines > 0)
            {
                std::vector<SDL_Color> textColors = {
                    {0, 0, 255, 255}, {0, 255, 0, 255}, {255, 0, 0, 255}, {255, 0, 255, 255},
                    {245, 159, 22, 255}, {22, 230, 245, 255}, {200, 200, 200, 255}, {150, 150, 150, 255}
                };

                SDL_Color textColor = textColors.at(cell.adjacentMines - 1);
                std::string text = std::to_string(cell.adjacentMines);

                int padding = 2;
                SDL_Rect cellRect;
                cellRect.w = GameUI::cellSize - padding;
                cellRect.h = GameUI::cellSize - padding;
                cellRect.x = 50 + GameUI::cellSize * i;
                cellRect.y = 50 + GameUI::cellSize * j;

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

// Draws a colored square in the cell (cellX, cellY)
void drawSquare(int cellX, int cellY, SDL_Color color)
{
    int padding = 2;
    
    SDL_Rect cellRect;
    cellRect.w = GameUI::cellSize - padding;
    cellRect.h = GameUI::cellSize - padding;
    cellRect.x = 50 + GameUI::cellSize * cellX;
    cellRect.y = 50 + GameUI::cellSize * cellY;
    
    SDL_SetRenderDrawColor(GameUI::renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(GameUI::renderer, &cellRect); 
}

void clickCell()
{
    int x;
    int y;
    if (GameUI::event.type == SDL_MOUSEBUTTONDOWN)
    {
        x = GameUI::event.button.x;
        y = GameUI::event.button.y;
    }
    else if (GameUI::event.type == SDL_KEYDOWN)
    {
        SDL_GetMouseState(&x, &y);
    }

    if (x < 50 || x > WINDOW_WIDTH - 50 || y < 50 || y > WINDOW_HEIGHT)
    {
        return;
    }
    
    int mouseX = x - 50;
    int mouseY = y - 50;
    
    int cellX = mouseX / GameUI::cellSize;
    int cellY = mouseY / GameUI::cellSize;

    if (!GameUI::board->isInBounds(cellX, cellY))
    {
        return;
    }
    
    Cell &cell = GameUI::board->getCell(cellX, cellY);
    if (cell.isFlagged)
    {
        return;
    }
    
    std::cout << "Clicked on cell " << cellX << "; " << cellY << std::endl;
    GameUI::nbClicks++;

    if (GameUI::nbClicks == 1)
    {
        GameUI::board->placeAllMines(cellX, cellY);
        GameUI::board->updateAllCellAdjacencies();
        GameUI::board->printBoard();
    }
    
    if (cell.isMine)
    {
        std::cout << "You clicked on a mine and lost !" << std::endl;
        cell.isRevealed = true;
        GameUI::isGameFinished = true;
        return;
    }
    if (cell.isRevealed)
    {
        chordCell(cellX, cellY);
        return;
    }

    revealCell(cellX, cellY);
    checkForGameFinish();
}

void revealCell(int x, int y)
{
    if (!GameUI::board->isInBounds(x, y))
    {
        return;
    }
    
    Cell &cell = GameUI::board->getCell(x, y);
    if (cell.isRevealed || cell.isMine)
    {
        return;
    }
    
    cell.isRevealed = true;

    SDL_Color emptyColor = {180, 180, 180, 255};
    drawSquare(x, y, emptyColor);

    if (cell.adjacentMines > 0)
    {
        std::vector<SDL_Color> textColors = {
            {0, 0, 255, 255}, {0, 255, 0, 255}, {255, 0, 0, 255}, {255, 0, 255, 255},
            {245, 159, 22, 255}, {22, 230, 245, 255}, {200, 200, 200, 255}, {150, 150, 150, 255}
        };

        SDL_Color textColor = textColors.at(cell.adjacentMines - 1);
        std::string text = std::to_string(cell.adjacentMines);

        int padding = 2;
        SDL_Rect cellRect;
        cellRect.w = GameUI::cellSize - padding;
        cellRect.h = GameUI::cellSize - padding;
        cellRect.x = 50 + GameUI::cellSize * x;
        cellRect.y = 50 + GameUI::cellSize * y;

        drawText(text, cellRect, textColor);
        return;
    }

    for (int dx = x - 1; dx <= x + 1; dx++)
    {
        for (int dy = y -1; dy <= y + 1; dy++)
        {
            if (dx == x && dy == y)
            {
                continue;
            }

            if (GameUI::board->isInBounds(dx, dy))
            {
                revealCell(dx, dy);
            }
        }
    }
}

void flagCell()
{
    int x;
    int y;
    if (GameUI::event.type == SDL_MOUSEBUTTONDOWN)
    {
        x = GameUI::event.button.x;
        y = GameUI::event.button.y;
    }
    else if (GameUI::event.type == SDL_KEYDOWN)
    {
        SDL_GetMouseState(&x, &y);
    }

    if (x < 50 || x > WINDOW_WIDTH - 50 || y < 50 || y > WINDOW_HEIGHT)
    {
        return;
    }

    int mouseX = x - 50;
    int mouseY = y - 50;

    int cellX = mouseX / GameUI::cellSize;
    int cellY = mouseY / GameUI::cellSize;

    if (!GameUI::board->isInBounds(cellX, cellY))
    {
        return;
    }
    
    std::cout << "Flagged cell " << cellX << "; " << cellY << std::endl;

    Cell &cell = GameUI::board->getCell(cellX, cellY);
    if (!cell.isRevealed)
    {
        cell.isFlagged = !cell.isFlagged; // Flag the cell if it was not flagged or unflag it if there was a flag
        return;
    }
}

void chordCell(int x, int y)
{
    Cell &chordCell = GameUI::board->getCell(x, y);
    if (!chordCell.isRevealed || chordCell.adjacentMines == 0)
    {
        return;
    }
    
    int totalFlaggedCellsCont = 0;
    
    for (int dx = x - 1; dx <= x + 1; dx++)
    {
        for (int dy = y - 1; dy <= y + 1; dy++)
        {
            if (GameUI::board->isInBounds(dx, dy))
            {
                Cell &cell = GameUI::board->getCell(dx, dy);

                if (dx == x && dy == y)
                {
                    continue; // Skip chorded cell
                }

                if (cell.isFlagged)
                {
                    totalFlaggedCellsCont++;
                }
            }
        }
    }

    if (totalFlaggedCellsCont != chordCell.adjacentMines)
    {
        return;
    }
    

    // Chord
    for (int dx = x -1; dx <= x + 1; dx++)
    {
        for (int dy = y - 1; dy <= y + 1; dy++)
        {
            if(GameUI::board->isInBounds(dx, dy))
            {
                Cell &cell = GameUI::board->getCell(dx, dy);

                if (cell.isRevealed || cell.isFlagged || dx == x && dy == y)
                {
                    continue;
                }

                // Misplaced flag -> Explode cell and lose the game
                if (cell.isMine)
                {
                    cell.isRevealed = true;
                    finishGame();
                    return;
                }

                // Reveal all other unflagged and unrevealed cells
                revealCell(dx, dy);
            }
        }
    }

    checkForGameFinish();
}

void drawTextureInCell(int cellX, int cellY, SDL_Texture *texture)
{
    if (!texture)
    {
        return;    
    }
    
    int padding = 2;
    SDL_Rect imageRect;
    imageRect.w = GameUI::cellSize - padding;
    imageRect.h = GameUI::cellSize - padding;
    imageRect.x = 50 + GameUI::cellSize * cellX;
    imageRect.y = 50 + GameUI::cellSize * cellY;

    SDL_RenderCopy(GameUI::renderer, texture, nullptr, &imageRect);
}

void checkForGameFinish()
{
    for (int i = 0; i < GameUI::board->width; i++)
    {
        for (int j = 0; j < GameUI::board->height; j++)
        {
            Cell &cell = GameUI::board->getCell(i, j);
            if (!cell.isMine && !cell.isRevealed)
            {
                return;
            }
        }
    }
    
    finishGame();
}

// Place all unplaced flags for cells that are mines but were not revealed
void finishGame()
{
    GameUI::isGameFinished = true;
    GameUI::isGameWon = true;
}

void resetBoard()
{
    int width = GameUI::board->width;
    int height = GameUI::board->height;
    int minesCount = GameUI::board->minesCount;
    *GameUI::board = Board(width, height, minesCount);
    GameUI::isGameFinished = false;
    GameUI::isGameWon = false;
    GameUI::nbClicks = 0;    
}
