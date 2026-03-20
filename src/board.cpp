#include <iostream>
#include "../include/board.hpp"

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"

#define COLOR_CELL_MINE "\033[91;41m"

// Beginner: 9x9/10
// Intermediate: 16x16/40
// Expert: 30x16/99

Board::Board(int w, int h, int mines) : rng(std::random_device{}())
{
    width = w;
    height = h;
    minesCount = mines;
    totalCells = width * height;
    if (minesCount > totalCells - 1)
    {
        std::cerr << "Board: Can't initialise board with no empty cells" << std::endl;
        exit(1);
    }
    
    remainingMines = minesCount;
    grid.resize(totalCells);

    for (Cell &cell : grid)
    {
        cell.isMine = false;
        cell.isRevealed = false;
        cell.isFlagged = false;
        cell.adjacentMines = 0;
    }
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
int Board::getRandomNumber(int max)
{
    std::uniform_int_distribution<int> dist(0, max - 1);
    return dist(rng);
}

void Board::placeRandomMine(int firstClickX, int firstClickY)
{
    int newMineIndex = -1;
    int attemptsCount = 0;

    int firstClickIndex = firstClickY * width + firstClickX;
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

void Board::expandConnectedZeroRegion(std::vector<bool> &visited, int cellX, int cellY)
{
    if (!isInBounds(cellX, cellY))
    {
        return;
    }

    int cellIndex = cellY * width + cellX;
    if (visited[cellIndex])
    {
        return;
    }
    
    Cell &cell = getCell(cellX, cellY);
    if (cell.isMine || cell.adjacentMines != 0)
    {
        return;
    }

    visited[cellIndex] = true; // Mark Zero cell as visited
    
    for (int dx = cellX - 1; dx <= cellX + 1; dx++)
    {
        for (int dy = cellY - 1; dy <= cellY + 1; dy++)
        {
            if (dx == cellX && dy == cellY)
            {
                continue;
            }
            
            expandConnectedZeroRegion(visited, dx, dy);
        }
    }
}

int Board::countZeroRegions()
{
    std::vector<bool> visited(totalCells, false);
    int zeroRegions = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Cell &cell = getCell(x, y);

            if (cell.isMine || cell.adjacentMines != 0)
            {
                continue;
            }

            int cellIndex = y * width + x;

            if (!visited[cellIndex])
            {
                zeroRegions++;
                expandConnectedZeroRegion(visited, x, y);
            }
        }
    }

    return zeroRegions;
}

bool Board::isAdjacentToZero(int cellX, int cellY)
{
    for (int dy = cellY - 1; dy <= cellY + 1; dy++)
    {
        for (int dx = cellX - 1; dx <= cellX + 1; dx++)
        {
            if (!isInBounds(dx, dy) || (dx == cellX && dy == cellY))
            {
                continue;
            }

            Cell &neighborCell = getCell(dx, dy);
            if (!neighborCell.isMine && neighborCell.adjacentMines == 0)
            {
                return true;
            }
        }
    }

    return false;
}

int Board::countIsolatedNumbers()
{
    int isolatedNumbers = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Cell &cell = getCell(x, y);

            if (cell.isMine || cell.adjacentMines == 0)
            {
                continue;
            }

            if (!isAdjacentToZero(x, y))
            {
                isolatedNumbers++;
            }
        }
    }

    return isolatedNumbers;
}

void Board::calculate3BV()
{
    // Count connected zero regions
    int zeroRegions = countZeroRegions();

    // Count isolated number cells not adjacent to zero regions
    int isolatedNumbers = countIsolatedNumbers();

    board3BV = zeroRegions + isolatedNumbers;
    std::cout << "Board 3BV: " << board3BV << " (Connected zero regions: " <<
        zeroRegions << ", Isolated numbers: " << isolatedNumbers << ")" << std::endl;
}
