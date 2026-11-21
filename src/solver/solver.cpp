#include "../../include/solver.hpp"
#include <algorithm>
#include <cmath>

Solver::Solver(Board &board) : board(board){};

std::vector<int> Solver::getAllConfigs(int cells, int flags)
{
    if (flags == 0)
    {
        return { 0 };
    }
    if (cells == flags)
    {
        return { (1 << cells) - 1};
    }
    std::vector<int> flagsFirst = getAllConfigs(cells - 1, flags - 1);
    std::vector<int> noFlagFirst = getAllConfigs(cells - 1, flags);
    
    int max = *std::max_element(flagsFirst.begin(), flagsFirst.end());
    int digits = max == 0 ? 1 : 1 + std::floor(std::log2(max)); // The floor is lava big dawg

    std::for_each(flagsFirst.begin(), flagsFirst.end(), [digits](int &i){i = (1 << digits) | i;});
    flagsFirst.insert(flagsFirst.end(), noFlagFirst.begin(), noFlagFirst.end());
    
    return flagsFirst;
}

bool Solver::isMinePlacementValid(int cellX, int cellY)
{
    if (!board.isInBounds(cellX, cellY))
    {
        return false;
    }

    int solvedAdjacentMinesCount = 0;
    for (int y = cellY - 1; y <= cellY + 1; y++)
    {
        for (int x = cellX - 1; x <= cellX + 1; x++)
        {
            if (!board.isInBounds(x, y) || (x == cellX && y == cellY))
            {
                continue;
            }

            Cell &cell = board.getCell(x, y);
            if (cell.isSolvedMine)
            {
                solvedAdjacentMinesCount++;
            }
        }
    }

    Cell &cell = board.getCell(cellX, cellY);
    return solvedAdjacentMinesCount == cell.adjacentMines;
}

void Solver::solveCell(int cellX, int cellY)
{
    std::vector<Cell> emptyCells = board.getAdjacentEmptyCells(cellX, cellY);
    int emptyCellsCount = emptyCells.size();

    Cell &cell = board.getCell(cellX, cellY);
    
    // Go through every configuration possible
    std::vector<int> configurations = getAllConfigs(emptyCellsCount, cell.adjacentMines);
    for (int config : configurations)
    {
        for (int bit = 0; bit < emptyCellsCount; bit++)
        {
            // Test all bits of the config and place flag if bit is set to 1
            if (config & (1 << bit))
            {
                emptyCells.at(emptyCellsCount - 1 - bit).isSolvedMine = true;
            }
        }
    }
}
