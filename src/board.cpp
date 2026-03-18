#include <iostream>
#include "../include/board.hpp"
#include <cstdlib>
#include <ctime>

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"

#define COLOR_CELL_MINE "\033[91;41m"

// Beginner: 9x9/10
// Intermediate: 16x16/40
// Expert: 30x16/99

Board::Board(int w, int h, int mines)
{
    width = w;
    height = h;
    minesCount = mines;
    totalCells = width * height;
    if (minesCount > totalCells - 1)
    {
        std::cerr << "Board: Can't initialise board with no empty cells" << std::endl;
        return;
    }
    
    remainingMines = minesCount;
    grid.resize(totalCells);
};

Cell& Board::getCell(int w, int h)
{
    return grid[h * width + w];
}

void Board::printBoard()
{
    std::cout << "Board of size " << width << "x" << height << "/" << minesCount << std::endl;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Cell cell = getCell(x, y);
            if (cell.isMine)
            {
                std::cout << COLOR_CELL_MINE "X" << COLOR_RESET << " ";
            }
            else
            {
                std::vector<std::string> colors = {"30;100", "94", "32", "91", "95", "33", "96", "37", "90"};
                std::cout << "\033[" << colors.at(cell.adjacentMines) << "m" << cell.adjacentMines << COLOR_RESET << " ";
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

void Board::placeRandomMine(int firstClickX, int firstClickY)
{
    int newMineIndex = -1;
    int attemptsCount = 0;

    int firstClickIndex = width * firstClickY + firstClickX;
    while (newMineIndex == -1 || newMineIndex == firstClickIndex  || grid[newMineIndex].isMine)
    {
        newMineIndex = getRandomNumber(totalCells);
        attemptsCount++;
    }

    std::cout << newMineIndex << "(" << attemptsCount << ") ";
    grid[newMineIndex].isMine = true;
}

void Board::placeAllMines(int firstClickX, int firstClickY)
{
    std::cout << "Placing all mines..." << std::endl;
    std::srand(std::time(0));
    std::cout << "Placing " << minesCount << " random mines : ";
    
    for (int i = 0; i < minesCount; i++)
    {
        placeRandomMine(firstClickX, firstClickY);
    }
    
    std::cout << std::endl;
}

bool Board::isInBounds(int w, int h)
{
    return w >= 0 && w < width && h >= 0 && h < height;
}

void Board::updateCellAdjacency(int w, int h)
{
    int totalAdjacentMines = 0;
    for (int y = h - 1; y <= h + 1; y++)
    {
        for (int x = w - 1; x <= w + 1; x++)
        {
            if (isInBounds(x, y) && getCell(x, y).isMine)
            {
                totalAdjacentMines++;
            }
        }
    }
    getCell(w, h).adjacentMines = totalAdjacentMines;
}

void Board::updateAllCellAdjacencies()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            updateCellAdjacency(x, y);
        }
    }
}
