#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hidapi.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>
#include <chrono>

#include "../include/ui.hpp"
#include "../include/draw.hpp"
#include "../include/timer.hpp"

SDL_Window *GameUI::window = nullptr;
SDL_Renderer *GameUI::renderer = nullptr;
TTF_Font *GameUI::cellFont = nullptr;
TTF_Font *GameUI::UIFont = nullptr;
SDL_Event GameUI::event;
Board *GameUI::board = nullptr;
int GameUI::cellSize = 40;
bool GameUI::isGameFinished = false;
bool GameUI::isGameWon = false;
SDL_Texture *GameUI::flagTexture = nullptr;
SDL_Texture *GameUI::mineTexture = nullptr;
int GameUI::nbClicks = 0;
int GameUI::leftClicks = 0;
int GameUI::rightClicks = 0;
int GameUI::chordClicks = 0;

const char clickCellKey = 'a';
const char flagCellKey = 'q';

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"

#define WINDOW_WIDTH 1850
#define WINDOW_HEIGHT 1020

#define GAME_INFO_OFFSET 300

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

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
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

    updateCellSize();
    int fontSize = GameUI::cellSize / 2;
    GameUI::cellFont = TTF_OpenFont("assets/fonts/minesweeper-font/minesweeper.otf", fontSize);
    if (!GameUI::cellFont)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(GameUI::renderer);
        SDL_DestroyWindow(GameUI::window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }

    GameUI::UIFont = TTF_OpenFont("assets/fonts/open-sans-font/open-sans.ttf", 25);
    if (!GameUI::UIFont)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_CloseFont(GameUI::cellFont);
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
        std::cout << "Failed to load flag texture: " << IMG_GetError() << std::endl;
        TTF_CloseFont(GameUI::cellFont);
        TTF_CloseFont(GameUI::UIFont);
        SDL_DestroyRenderer(GameUI::renderer);
        SDL_DestroyWindow(GameUI::window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }
    GameUI::mineTexture = IMG_LoadTexture(GameUI::renderer, "assets/mine.png");
    if (!GameUI::mineTexture)
    {
        std::cout << "Failed to load mine texture: " << IMG_GetError() << std::endl;
        TTF_CloseFont(GameUI::cellFont);
        TTF_CloseFont(GameUI::UIFont);
        SDL_DestroyRenderer(GameUI::renderer);
        SDL_DestroyWindow(GameUI::window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return;
    }
        
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
            if ((GameUI::event.type == SDL_MOUSEBUTTONUP && GameUI::event.button.button == SDL_BUTTON_LEFT) ||
                (GameUI::event.type == SDL_KEYUP && GameUI::event.key.keysym.sym == SDLK_a))
            {
                clickCell();
            }
            
            // Flag cell with right click or "Q"
            if ((GameUI::event.type == SDL_MOUSEBUTTONDOWN && GameUI::event.button.button == SDL_BUTTON_RIGHT) ||
                (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_q))
            {
                flagCell();
            }

            // Reset board
            if (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_SPACE)
            {
                resetBoard();
            }
        }

        if (Timer::updateTimer())
        {
            //std::cout << "Elapsed time: " << GameUI::elapsedSeconds << std::endl;
            Draw::drawGameInfo();
            SDL_RenderPresent(GameUI::renderer);
        }
        Draw::renderFrame();
        
        SDL_Delay(16);
    }

    if (GameUI::flagTexture)
    {
        SDL_DestroyTexture(GameUI::flagTexture);
    }
    if (GameUI::mineTexture)
    {
        SDL_DestroyTexture(GameUI::mineTexture);
    }
    
    TTF_CloseFont(GameUI::cellFont);
    TTF_CloseFont(GameUI::UIFont);

    SDL_DestroyRenderer(GameUI::renderer);
    SDL_DestroyWindow(GameUI::window);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void updateCellSize()
{
    const int window_offset = 50;

    int availableWidth = (WINDOW_WIDTH - GAME_INFO_OFFSET)  - window_offset * 2;
    int availableHeight = WINDOW_HEIGHT - window_offset * 2;
    
    int cellSizeX = availableWidth / GameUI::board->width;
    int cellSizeY = availableHeight / GameUI::board->height;

    if (cellSizeX < cellSizeY)
    {
        GameUI::cellSize = cellSizeX;
    }
    else
    {
        GameUI::cellSize = cellSizeY;
    }
}

void clickCell()
{
    if (GameUI::isGameFinished)
    {
        return;
    }
    
    int x = 0;
    int y = 0;
    if (GameUI::event.type == SDL_MOUSEBUTTONUP)
    {
        x = GameUI::event.button.x;
        y = GameUI::event.button.y;
    }
    else if (GameUI::event.type == SDL_KEYUP)
    {
        SDL_GetMouseState(&x, &y);
    }
    else
    {
        return;
    }

    if (x < 50 || x >= 50 + GameUI::cellSize * GameUI::board->width
        || y < 50 || y >= 50 + GameUI::cellSize * GameUI::board->height)
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
    
    // std::cout << "Clicked on cell " << cellX << "; " << cellY << std::endl;

    if (GameUI::nbClicks == 0)
    {
        GameUI::board->placeAllMines(cellX, cellY);
        GameUI::board->updateAllCellAdjacencies();
        GameUI::board->printBoard();
        Timer::startTimer();
    }

    // Clicked on a mine: Lose game
    if (cell.isMine)
    {
        GameUI::nbClicks++;
        GameUI::leftClicks++;
        cell.isRevealed = true;
        finishGame(false);
        return;
    }

    // Clicked on an already revealed cell : Chord cell
    if (cell.isRevealed)
    {
        chordCell(cellX, cellY);
        return;
    }

    GameUI::nbClicks++;
    GameUI::leftClicks++;
    revealCell(cellX, cellY);
    checkForGameFinish();
}

void revealCell(int cellX, int cellY)
{
    if (!GameUI::board->isInBounds(cellX, cellY))
    {
        return;
    }
    
    Cell &cell = GameUI::board->getCell(cellX, cellY);
    if (cell.isRevealed || cell.isMine || cell.isFlagged)
    {
        return;
    }
    
    cell.isRevealed = true;

    // Stop on numbered cells
    if (cell.adjacentMines > 0)
    {
        return;
    }

    // Expand only from 0-mines cells
    for (int dx = cellX - 1; dx <= cellX + 1; dx++)
    {
        for (int dy = cellY -1; dy <= cellY + 1; dy++)
        {
            if (dx == cellX && dy == cellY)
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
    if (GameUI::isGameFinished)
    {
        return;
    }

    int x = 0;
    int y = 0;
    if (GameUI::event.type == SDL_MOUSEBUTTONDOWN)
    {
        x = GameUI::event.button.x;
        y = GameUI::event.button.y;
    }
    else if (GameUI::event.type == SDL_KEYDOWN)
    {
        SDL_GetMouseState(&x, &y);
    }
    else
    {
        return;
    }

    if (x < 50 || x >= 50 + GameUI::cellSize * GameUI::board->width || y < 50 || y >= 50 + GameUI::cellSize * GameUI::board->height)
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
    
    // std::cout << "Flagged cell " << cellX << "; " << cellY << std::endl;

    if (GameUI::nbClicks == 0)
    {
        GameUI::board->placeAllMines(-1, -1);
        GameUI::board->updateAllCellAdjacencies();
        GameUI::board->printBoard();
        Timer::startTimer();
    }

    GameUI::nbClicks++;
    GameUI::rightClicks++;

    Cell &cell = GameUI::board->getCell(cellX, cellY);
    if (!cell.isRevealed)
    {
        cell.isFlagged = !cell.isFlagged; // Flag the cell if it was not flagged or unflag it if there was a flag
        if (cell.isFlagged)
        {
            GameUI::board->remainingMines--;
        }
        else
        {
            GameUI::board->remainingMines++;
        }
        
        return;
    }
}

void chordCell(int cellX, int cellY)
{
    Cell &chordCell = GameUI::board->getCell(cellX, cellY);
    if (!chordCell.isRevealed || chordCell.adjacentMines == 0)
    {
        return;
    }

    GameUI::nbClicks++;
    GameUI::chordClicks++;
    
    int totalFlaggedCellsCont = 0;
    
    for (int dx = cellX - 1; dx <= cellX + 1; dx++)
    {
        for (int dy = cellY - 1; dy <= cellY + 1; dy++)
        {
            if (GameUI::board->isInBounds(dx, dy))
            {
                Cell &cell = GameUI::board->getCell(dx, dy);

                if (dx == cellX && dy == cellY)
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
    for (int dx = cellX -1; dx <= cellX + 1; dx++)
    {
        for (int dy = cellY - 1; dy <= cellY + 1; dy++)
        {
            if(GameUI::board->isInBounds(dx, dy))
            {
                Cell &cell = GameUI::board->getCell(dx, dy);

                if (cell.isRevealed || cell.isFlagged || dx == cellX && dy == cellY)
                {
                    continue;
                }

                // Misplaced flag -> Explode cell and lose the game
                if (cell.isMine)
                {
                    cell.isRevealed = true;
                    finishGame(false);
                    continue;
                }

                // Reveal all other unflagged and unrevealed cells
                revealCell(dx, dy);
            }
        }
    }

    checkForGameFinish();
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
    
    finishGame(true);
}

// Place all unplaced flags for cells that are mines but were not revealed
void finishGame(bool isWon)
{
    Timer::endTimer();
    
    GameUI::board->solvingTime = Timer::finalTimeSeconds + (float) Timer::finalTimeMilliseconds / 1000;
    GameUI::board->solved3BV = GameUI::board->calculate3BV(isWon);
    GameUI::board->total3BV = GameUI::board->calculate3BV(true);

    GameUI::board->solved3BVPerSecond = GameUI::board->solved3BV / GameUI::board->solvingTime;
    GameUI::board->totalClicks = GameUI::nbClicks;
    GameUI::board->leftClicks = GameUI::leftClicks;
    GameUI::board->rightClicks = GameUI::rightClicks;
    GameUI::board->chordClicks = GameUI::chordClicks;
    GameUI::board->efficiency = (float) GameUI::board->solved3BV / GameUI::board->totalClicks * 100;
    
    GameUI::isGameFinished = true;
    GameUI::isGameWon = isWon;

    if (isWon)
    {
        GameUI::board->remainingMines = 0;
        std::cout << COLOR_GREEN << "Finished board in ";
    }
    else
    {
        std::cout << COLOR_RED << "Failed board in ";
    }

    std::cout << Timer::finalTimeSeconds << "." << Timer::finalTimeMilliseconds << " sec" << std::endl << COLOR_RESET;

    if (isWon)
    {
        std::cout << "3BV: " << GameUI::board->solved3BV << std::endl; 
    }
    else
    {
        std::cout << "3BV: " << GameUI::board->solved3BV << " / " << GameUI::board->total3BV << std::endl;
    }

    char solved3BVPerSecondBuffer[20];
    snprintf(solved3BVPerSecondBuffer, sizeof(solved3BVPerSecondBuffer), "3BV/s: %.3f", GameUI::board->solved3BVPerSecond);
    std::string solved3BVPerSecondString = solved3BVPerSecondBuffer;
    std::cout << solved3BVPerSecondString << std::endl;
    
    std::cout << "Clicks: " << GameUI::nbClicks << " (Left clicks: " <<
        GameUI::leftClicks << ", Right clicks: " << GameUI::rightClicks <<
        ", Chord clicks: " << GameUI::chordClicks << ")" << std::endl;
    std::cout << "Efficiency: " << GameUI::board->efficiency << "%" << std::endl;
}

void resetBoard()
{
    int width = GameUI::board->width;
    int height = GameUI::board->height;
    int minesCount = GameUI::board->minesCount;
    
    delete GameUI::board;
    GameUI::board = new Board(width, height, minesCount);

    if (!GameUI::board)
    {
        std::cerr << "ResetBoard: Failed to reset board" << std::endl;
    }
    
    GameUI::isGameFinished = false;
    GameUI::isGameWon = false;
    GameUI::nbClicks = 0;
    GameUI::leftClicks = 0;
    GameUI::rightClicks = 0;
    GameUI::chordClicks = 0;
    
    Timer::resetTimer();
    Draw::drawGameInfo();
}
