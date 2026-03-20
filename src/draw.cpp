#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include "../include/ui.hpp"
#include "../include/draw.hpp"
#include "../include/timer.hpp"

#define WINDOW_WIDTH 1850
#define WINDOW_HEIGHT 1020

#define GAME_INFO_OFFSET 300

// Called at the start of the game to draw borders
void Draw::drawStaticUI()
{
    // TODO: Draw borders at real board borders but still make sure to respect these borders

    if (SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_SetRenderDrawColor failed: " << SDL_GetError() << std::endl;
    }
    
    if (SDL_RenderClear(GameUI::renderer) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_RenderClear failed: " << SDL_GetError() << std::endl;
    }

    if (SDL_SetRenderDrawColor(GameUI::renderer, 255, 255, 255, 255) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_SetRenderDrawColor failed: " << SDL_GetError() << std::endl;
    }

    int window_offset = 40;

    // Draw borders
    // Top line
    if (SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, WINDOW_WIDTH - GAME_INFO_OFFSET, window_offset) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_RenderDrawLine failed: " << SDL_GetError() << std::endl;
    }

    // Left line
    if (SDL_RenderDrawLine(GameUI::renderer, window_offset, window_offset, window_offset, WINDOW_HEIGHT - window_offset) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_RenderDrawLine failed: " << SDL_GetError() << std::endl;
    }

    // Right line
    if (SDL_RenderDrawLine(GameUI::renderer, WINDOW_WIDTH - GAME_INFO_OFFSET, window_offset, WINDOW_WIDTH - GAME_INFO_OFFSET,
                       WINDOW_HEIGHT - window_offset) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_RenderDrawLine failed: " << SDL_GetError() << std::endl;
    }

    // Bottom line
    if (SDL_RenderDrawLine(GameUI::renderer, window_offset, WINDOW_HEIGHT - window_offset, WINDOW_WIDTH - GAME_INFO_OFFSET,
                       WINDOW_HEIGHT - window_offset) < 0)
    {
        std::cerr << "Draw: DrawStaticUI: SDL_RenderDrawLine failed: " << SDL_GetError() << std::endl;
    }
}

// Called when board state changes (on click)
void Draw::redrawBoardUI()
{
    updateCellSize();
    if (GameUI::cellSize < 3)
    {
        GameUI::cellSize = 3;
    }
    
    SDL_Rect boardRect;
    boardRect.x = 50;
    boardRect.y = 50;
    boardRect.w = GameUI::cellSize * GameUI::board->width;
    boardRect.h = GameUI::cellSize * GameUI::board->height;

    // Clear only board area
    if (SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255) < 0)
    {
        std::cerr << "Draw: RedrawBoardUI: SDL_SetRenderDrawColor failed: " << SDL_GetError() << std::endl;
    }
    
    if (SDL_RenderFillRect(GameUI::renderer, &boardRect) < 0)
    {
        std::cerr << "Draw: RedrawBoardUI: SDL_RenderFillRect failed: " << SDL_GetError() << std::endl;
    }

    drawAllCells();
}

void Draw::drawTextInCell(const std::string &text, SDL_Rect cellRect, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(GameUI::cellFont, text.c_str(), color);
    if (!surface)
    {
        std::cerr << "Draw: DrawTextInCell: TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(GameUI::renderer, surface);
    if (!texture)
    {
        std::cerr << "Draw: DrawTextInCell: SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect textRect;
    textRect.w = surface->w;
    textRect.h = surface->h;
    textRect.x = cellRect.x + (cellRect.w - textRect.w) / 2;
    textRect.y = cellRect.y + (cellRect.h - textRect.h) / 2;

    if (SDL_RenderCopy(GameUI::renderer, texture, nullptr, &textRect) < 0)
    {
        std::cerr << "Draw: DrawTextInCell: SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Draw::drawText(const std::string &text, SDL_Rect textRect, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(GameUI::UIFont, text.c_str(), color);
    if (!surface)
    {
        std::cerr << "Draw: DrawText: TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(GameUI::renderer, surface);
    if (!texture)
    {
        std::cerr << "Draw: DrawText: SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect finalTextRect;
    finalTextRect.w = surface->w;
    finalTextRect.h = surface->h;
    finalTextRect.x = textRect.x;
    finalTextRect.y = textRect.y;

    if (SDL_RenderCopy(GameUI::renderer, texture, nullptr, &finalTextRect) < 0)
    {
        std::cerr << "Draw: DrawText: SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Draw::drawAllCells()
{
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
                
                // Misplaced flag at game loss
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

void Draw::drawNumber(int cellX, int cellY)
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

void Draw::drawGameStatistics()
{
    SDL_Rect statsRect;
    statsRect.x = 10;
    statsRect.y = 10;
    statsRect.w = 400;
    statsRect.h = 30;

    SDL_Color statsTextColor = {255, 255, 0};

    char statsBuffer[100];
    snprintf(statsBuffer, sizeof(statsBuffer), "Board size: %ix%i/%i, Mine density: %.2f%s",
             GameUI::board->width, GameUI::board->height, GameUI::board->minesCount,
             (float) GameUI::board->minesCount / GameUI::board->totalCells * 100, "%");
    std::string statsText = statsBuffer;
    drawText(statsText, statsRect, statsTextColor);
}

void Draw::drawGameInfo()
{
    SDL_Rect resetRect;
    resetRect.x = 600;
    resetRect.y = 10;
    resetRect.w = 1000;
    resetRect.h = 30;

    if (SDL_SetRenderDrawColor(GameUI::renderer, 0, 0, 0, 255) < 0)
    {
        std::cerr << "Draw: DrawGameInfo: SDL_SetRenderDrawColor failed: " << SDL_GetError() << std::endl;
    }

    if (SDL_RenderFillRect(GameUI::renderer, &resetRect) < 0)
    {
        std::cerr << "Draw: DrawGameInfo: SDL_RenderFillRect failed: " << SDL_GetError() << std::endl;
    }
    
    SDL_Color infoTextColor = {255, 0, 0};
    
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

void Draw::drawGameFinishInfo()
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
             GameUI::chordClicks);
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
        GameUI::leftClicks << ", Right clicks: " << GameUI::rightClicks <<
        ", Chord clicks: " << GameUI::chordClicks << ")" << std::endl;

    SDL_Color gameFinishTextColor = {255, 255, 255};

    drawText(timeText, timerRect, gameFinishTextColor);
    drawText(clicksText, clicksRect, gameFinishTextColor);
    drawText(leftClicksText, leftClicksRect, gameFinishTextColor);
    drawText(rightClicksText, rightClicksRect, gameFinishTextColor);
    drawText(chordClicksText, chordClicksRect, gameFinishTextColor);
}

// Draws a colored square in the cell (cellX, cellY)
void Draw::drawSquare(int cellX, int cellY, SDL_Color color)
{
    int padding = 2;
    
    SDL_Rect cellRect;
    cellRect.w = GameUI::cellSize - padding;
    cellRect.h = GameUI::cellSize - padding;
    cellRect.x = 50 + GameUI::cellSize * cellX;
    cellRect.y = 50 + GameUI::cellSize * cellY;
    
    if (SDL_SetRenderDrawColor(GameUI::renderer, color.r, color.g, color.b, color.a) < 0)
    {
        std::cerr << "Draw: DrawSquare: SDL_SetRenderDrawColor failed: " << SDL_GetError() << std::endl;
        return;
    }
    
    if (SDL_RenderFillRect(GameUI::renderer, &cellRect) < 0)
    {
        std::cerr << "Draw: DrawSquare: SDL_RenderFillRect failed: " << SDL_GetError() << std::endl;
    } 
}

void Draw::drawTextureInCell(int cellX, int cellY, SDL_Texture *texture)
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

    if (SDL_RenderCopy(GameUI::renderer, texture, nullptr, &imageRect) < 0)
    {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }
}
