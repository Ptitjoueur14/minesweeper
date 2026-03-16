#include <iostream>
#include "../include/board.hpp"
#include <cstdlib>
#include <ctime>

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"

Board::Board(int w, int h, int mines)
{
    width = w;
    height = h;
    minesCount = mines;
    totalCells = width * height;
    remainingMines = minesCount;
    grid.resize(totalCells);
};

Cell& Board::getCell(int x, int y)
{
    return grid[y * width + x];
}

void Board::printBoard()
{
    std::cout << "Board of size " << width << "x" << height << "/" << minesCount << std::endl;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (getCell(x, y).isMine)
            {
                std::cout << COLOR_RED "X";
            }
            else
            {
                std::cout << COLOR_GREEN "O";
            }
        }
        std::cout << COLOR_RESET << std::endl;
    }
}

// Gets a random number between 0 and max
// Uses classic modulo (biased) method.
int Board::getRandomNumber(int max)
{
    return std::rand() % max;
}

void Board::placeRandomMine()
{
    int newMineIndex = -1;
    while (newMineIndex == -1 || grid[newMineIndex].isMine)
    {
        newMineIndex = getRandomNumber(totalCells);
    }
    std::cout << newMineIndex << " ";
    grid[newMineIndex].isMine = true;
}

void Board::placeAllMines()
{
    std::cout << "Placing all mines..." << std::endl;
    std::srand(std::time(0));
    std::cout << "Placing " << minesCount << " random mines : ";
    for (int i = 0; i < minesCount; i++)
    {
        placeRandomMine();
    }
    std::cout << std::endl;
}
