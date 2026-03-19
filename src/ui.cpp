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
#include "../include/timer.hpp"

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
int GameUI::leftClicks = 0;
int GameUI::rightClicks = 0;
int GameUI::chordClicks = 0;

const char clickCellKey = 'a';
const char flagCellKey = 'q';

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
        std::cout << "Failed to load flag texture: " << IMG_GetError() << std::endl;
        return;
    }
    GameUI::mineTexture = IMG_LoadTexture(GameUI::renderer, "assets/mine.png");
    if (!GameUI::mineTexture)
    {
        std::cout << "Failed to load mine texture: " << IMG_GetError() << std::endl;
        return;
    }
    
    drawStaticUI();
    drawGameInfo();    
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
                drawGameInfo();
                SDL_RenderPresent(GameUI::renderer);
            }
            
            // Flag cell with right click or "Q"
            if ((GameUI::event.type == SDL_MOUSEBUTTONDOWN && GameUI::event.button.button == SDL_BUTTON_RIGHT) ||
                (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_q))
            {
                flagCell();
                redrawBoardUI();
                drawGameInfo();
                SDL_RenderPresent(GameUI::renderer);
            }

            // Reset board
            if (GameUI::event.type == SDL_KEYDOWN && GameUI::event.key.keysym.sym == SDLK_SPACE)
            {
                resetBoard();
                drawStaticUI();
                drawGameStatistics();
                drawGameInfo();
                redrawBoardUI();
                SDL_RenderPresent(GameUI::renderer);
            }
        }

        
        if (Timer::updateTimer())
        {
            //std::cout << "Elapsed time: " << GameUI::elapsedSeconds << std::endl;
            drawGameInfo();
            SDL_RenderPresent(GameUI::renderer);
        }
        
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
    // TODO: Draw borders at real board borders but still make sure to respect these borders

    SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255);
    SDL_RenderClear(GameUI::renderer);

    SDL_SetRenderDrawColor(GameUI::renderer, 255, 255, 255, 255);

    int window_offset = 40;

    // Draw borders
    // Top line
    SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, WINDOW_WIDTH - GAME_INFO_OFFSET, window_offset);

    // Left line
    SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, window_offset, WINDOW_HEIGHT - window_offset);

    // Right line
    SDL_RenderDrawLine(GameUI::renderer, WINDOW_WIDTH - GAME_INFO_OFFSET, window_offset, WINDOW_WIDTH - GAME_INFO_OFFSET,
                       WINDOW_HEIGHT - window_offset);

    // Bottom line
    SDL_RenderDrawLine(GameUI::renderer, window_offset, WINDOW_HEIGHT - window_offset, WINDOW_WIDTH - GAME_INFO_OFFSET,
                       WINDOW_HEIGHT - window_offset);
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

void drawTextInCell(const std::string &text, SDL_Rect cellRect, SDL_Color color)
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

void drawText(const std::string &text, SDL_Rect textRect, SDL_Color color)
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
    
    SDL_Rect finalTextRect;
    finalTextRect.w = surface->w;
    finalTextRect.h = surface->h;
    finalTextRect.x = textRect.x;
    finalTextRect.y = textRect.y;

    SDL_RenderCopy(GameUI::renderer, texture, nullptr, &finalTextRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void drawAllCells()
{
    int window_offset = 50;

    int cellSizeX = (WINDOW_WIDTH - window_offset - GAME_INFO_OFFSET) / GameUI::board->width;
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

            if (GameUI::isGameFinished)
            {
                // Exploded/Revealed mine
                if (cell.isMine && cell.isRevealed)
                {
                    SDL_Color mineColor = {255, 0, 0, 255};
                    drawSquare(i, j, mineColor);
                    drawTextureInCell(i, j, GameUI::mineTexture);
                    continue;
                }
                
                // Missplaced flag at game loss
                if (!GameUI::isGameWon && cell.isFlagged && !cell.isMine)
                {
                    SDL_Color misplacedFlagColor = {255, 0, 0, 255};
                    drawSquare(i, j, misplacedFlagColor);
                    drawTextureInCell(i, j, GameUI::flagTexture);
                    continue;
                }
                
                // Unrevealed mine at game win (place flag)
                if (GameUI::isGameWon && cell.isMine && !cell.isRevealed)
                {
                    SDL_Color hiddenColor = {120, 120, 120, 255};
                    drawSquare(i, j, hiddenColor);
                    drawTextureInCell(i, j, GameUI::flagTexture);
                    continue;
                }
                
                // Unplaced mine at game loss (place mine)
                if (!GameUI::isGameWon && cell.isMine && !cell.isRevealed && !cell.isFlagged)
                {
                    SDL_Color hiddenColor = {120, 120, 120, 255};
                    drawSquare(i, j, hiddenColor);
                    drawTextureInCell(i, j, GameUI::mineTexture);
                    continue;
                }
            }
            
            // Hidden cell
            if (!cell.isRevealed && !cell.isFlagged)
            {
                SDL_Color hiddenColor = {120, 120, 120, 255};
                drawSquare(i, j, hiddenColor);
                continue;
            }

            // Flag
            if (cell.isFlagged)
            {
                SDL_Color hiddenColor = {120, 120, 120, 255};
                drawSquare(i, j, hiddenColor);
                drawTextureInCell(i, j, GameUI::flagTexture);
                continue;
            }

            // Reveal normal cell with number
            SDL_Color revealedColor = {180, 180, 180, 255};
            drawSquare(i, j, revealedColor);
            drawNumber(i, j);
        }
    }
}

void drawNumber(int cellX, int cellY)
{
    Cell &cell = GameUI::board->getCell(cellX, cellY);
    
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
        cellRect.x = 50 + GameUI::cellSize * cellX;
        cellRect.y = 50 + GameUI::cellSize * cellY;

        drawTextInCell(text, cellRect, textColor);
    }
}

void drawGameStatistics()
{
    SDL_Rect statsRect;
    statsRect.x = 10;
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

void drawGameInfo()
{
    SDL_Rect resetRect;
    resetRect.x = 600;
    resetRect.y = 0;
    resetRect.w = 1000;
    resetRect.h = 38;

    SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(GameUI::renderer, &resetRect);
    
    SDL_Color infoTextColor = {255, 0, 0};
    int fontSize = 15;
    TTF_SetFontSize(GameUI::font, fontSize);
    
    SDL_Rect minesRect;
    minesRect.x = 800;
    minesRect.y = 10;
    minesRect.w = 100;
    minesRect.h = 30;

    SDL_Rect timerRect;
    timerRect.x = 1200;
    timerRect.y = 10;
    timerRect.w = 100;
    timerRect.h = 30;

    std::string minesText = std::to_string(GameUI::board->remainingMines);
    std::string timerText = std::to_string(Timer::elapsedSeconds);

    drawText(minesText, minesRect, infoTextColor);
    drawText(timerText, timerRect, infoTextColor);
}

void drawGameFinishInfo()
{
    int timeSeconds = Timer::finalTimeSeconds;
    int timeMilliseconds = Timer::finalTimeMilliseconds;

    char timeBuffer[100];
    snprintf(timeBuffer, sizeof(timeBuffer), "Time: %i.%i s", timeSeconds, timeMilliseconds);
    std::string timeText = timeBuffer;

    char clicksBuffer[100];
    snprintf(clicksBuffer, sizeof(clicksBuffer), "Clicks: %i",
             GameUI::nbClicks);
    std::string clicksText = clicksBuffer;
    
    snprintf(clicksBuffer, sizeof(clicksBuffer), "Left clicks: %i",
             GameUI::leftClicks);
    std::string leftClicksText = clicksBuffer;
    
    snprintf(clicksBuffer, sizeof(clicksBuffer), "Right clicks: %i",
             GameUI::rightClicks);
    std::string rightClicksText = clicksBuffer;
    
    snprintf(clicksBuffer, sizeof(clicksBuffer), "Chord clicks: %i",
             GameUI::rightClicks);
    std::string chordClicksText = clicksBuffer;

    SDL_Rect timerRect;
    timerRect.x = (WINDOW_WIDTH - GAME_INFO_OFFSET) + 30;
    timerRect.y = GAME_INFO_OFFSET;
    timerRect.w = 150;
    timerRect.h = 50;
    
    SDL_Rect clicksRect;
    clicksRect.x = (WINDOW_WIDTH - GAME_INFO_OFFSET) + 30;
    clicksRect.y = GAME_INFO_OFFSET + 50;
    clicksRect.w = 300;
    clicksRect.h = 50;
    
    SDL_Rect leftClicksRect;
    leftClicksRect.x = (WINDOW_WIDTH - GAME_INFO_OFFSET) + 30;
    leftClicksRect.y = GAME_INFO_OFFSET + 80;
    leftClicksRect.w = 300;
    leftClicksRect.h = 50;
    
    SDL_Rect rightClicksRect;
    rightClicksRect.x = (WINDOW_WIDTH - GAME_INFO_OFFSET) + 30;
    rightClicksRect.y = GAME_INFO_OFFSET + 110;
    rightClicksRect.w = 300;
    rightClicksRect.h = 50;
    
    SDL_Rect chordClicksRect;
    chordClicksRect.x = (WINDOW_WIDTH - GAME_INFO_OFFSET) + 30;
    chordClicksRect.y = GAME_INFO_OFFSET + 140;
    chordClicksRect.w = 300;
    chordClicksRect.h = 50;

    std::cout << "Clicks : " << GameUI::nbClicks << " (Left clicks: " <<
        GameUI::leftClicks << ", Right clicks: " << GameUI::rightClicks << ", Chord clicks: " << GameUI::chordClicks << ")" << std::endl;

    SDL_Color gameFinishTextColor = {255, 255, 255};
    drawText(timeText, timerRect, gameFinishTextColor);
    drawText(clicksText, clicksRect, gameFinishTextColor);
    drawText(leftClicksText, leftClicksRect, gameFinishTextColor);
    drawText(rightClicksText, rightClicksRect, gameFinishTextColor);
    drawText(chordClicksText, chordClicksRect, gameFinishTextColor);
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
    if (GameUI::isGameFinished)
    {
        return;
    }
    
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

    if (GameUI::leftClicks == 0)
    {
        GameUI::board->placeAllMines(cellX, cellY);
        GameUI::board->updateAllCellAdjacencies();
        GameUI::board->printBoard();
        Timer::startTimer();
    }
    
    if (cell.isMine)
    {
        GameUI::nbClicks++;
        GameUI::leftClicks++;
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
    if (cell.isRevealed || cell.isMine)
    {
        return;
    }
    
    cell.isRevealed = true;

    SDL_Color emptyColor = {180, 180, 180, 255};
    drawSquare(cellX, cellY, emptyColor);

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
        cellRect.x = 50 + GameUI::cellSize * cellX;
        cellRect.y = 50 + GameUI::cellSize * cellY;

        drawText(text, cellRect, textColor);
        return;
    }

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
    GameUI::nbClicks++;
    GameUI::rightClicks++;

    if (GameUI::nbClicks == 1)
    {
        Timer::startTimer();
    }

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
    
    finishGame(true);
}

// Place all unplaced flags for cells that are mines but were not revealed
void finishGame(bool isWon)
{
    GameUI::isGameFinished = true;
    GameUI::isGameWon = isWon;

    if (isWon)
    {
        GameUI::board->remainingMines = 0;
    }

    Timer::endTimer();
    std::cout << "Finished game in " << Timer::finalTimeSeconds << "." << Timer::finalTimeMilliseconds << "s" << std::endl;

    drawGameFinishInfo();
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
    GameUI::leftClicks = 0;
    GameUI::rightClicks = 0;
    Timer::resetTimer();
    drawGameInfo();
}
