#include "../../include/solver.hpp"
#include "../../include/draw.hpp"
#include <cstddef>
#include <iostream>

Solver::Solver(Board &board) : board(board){};

long double Solver::choose(int n, int k)
{
    if (n < 0 || k < 0 || k > n)
    {
        return 0;
    }

    k = k < n - k ? k : n - k;
    long double result = 1;

    for (int i = 1; i <= k; i++)
    {
        result *= (n - k + i);
        result /= i;
    }

    return result;    
}

std::vector<Cell> Solver::getFrontierCells()
{
    std::vector<Cell> frontierCells;

    for (int y = 0; y < board.height; y++)
    {
        for (int x = 0; x < board.width; x++)
        {
            Cell &cell = board.getCell(x, y);
            if (cell.isRevealed && cell.visualState != EXPLODED)
            {
                continue;
            }

            bool adjacentToRevealed = false;

            for (int dy = y - 1; dy <= y + 1; dy++)
            {
                for (int dx = x - 1; dx <= x + 1; dx++)
                {
                    if (!board.isInBounds(dx, dy))
                    {
                        continue;
                    }

                    Cell& neighbor = board.getCell(dx, dy);
                    if (neighbor.isRevealed && neighbor.visualState != EXPLODED)
                    {
                        adjacentToRevealed = true;
                        break;
                    }
                }

                if (adjacentToRevealed)
                {
                    break;
                }
            }

            if (adjacentToRevealed)
            {
                frontierCells.push_back(cell);
            }
        }
    }

    return frontierCells;
}

std::vector<Cell> Solver::getHiddenCells()
{
    std::vector<Cell> hiddenCells;

    for (int y = 0; y < board.height; y++)
    {
        for (int x = 0; x < board.width; x++)
        {
            Cell &cell = board.getCell(x, y);
            if (cell.isRevealed && cell.visualState != EXPLODED)
            {
                continue;
            }

            bool adjacentToRevealed = false;
            
            for (int dy = y - 1; dy <= y + 1; dy++)
            {
                for (int dx = x - 1; dx <= x + 1; dx++)
                {
                    if (!board.isInBounds(dx, dy))
                    {
                        continue;
                    }
                    
                    Cell &neighborCell = board.getCell(dx, dy);
                    if (neighborCell.isRevealed && neighborCell.visualState != EXPLODED)
                    {
                        adjacentToRevealed = true;
                        break;
                    }
                }

                if (adjacentToRevealed)
                {
                    break;
                }
            }

            if (!adjacentToRevealed)
            {
                hiddenCells.push_back(cell);
            }
        }
    }

    return hiddenCells;
}

bool Solver::isConfigAssignementValid(const std::vector<Cell>& unknownCells, const std::vector<bool>& config, int index)
{
    for (int y = 0; y < board.height; y++)
    {
        for (int x = 0; x < board.width; x++)
        {
            Cell& cell = board.getCell(x, y);

            if (!cell.isRevealed || cell.visualState == EXPLODED)
            {
                continue;
            }

            int mines = 0;
            int remaining = 0;

            for (int dy = y - 1; dy <= y + 1; dy++)
            {
                for (int dx = x - 1; dx <= x + 1; dx++)
                {
                    if (!board.isInBounds(dx, dy) || (dx == x && dy == y))
                    {
                        continue;
                    }

                    for (size_t i = 0; i < unknownCells.size(); i++)
                    {
                        if (unknownCells.at(i).x == dx && unknownCells.at(i).y == dy)
                        {
                            if (i <= index)
                            {
                                if (config.at(i))
                                {
                                    mines++;
                                }
                            }
                            else
                            {
                                remaining++;
                            }

                            break;
                        }
                    }
                }
            }

            if (mines > cell.adjacentMines || mines + remaining < cell.adjacentMines)
            {
                return false;
            }
        }
    }

    return true;
}

bool Solver::isConfigurationValid(const std::vector<Cell>& unknownCells, const std::vector<bool>& config)
{
    for (int y = 0; y < board.height; y++)
    {
        for (int x = 0; x < board.width; x++)
        {
            Cell &cell = board.getCell(x, y);
            if (!cell.isRevealed || cell.visualState == EXPLODED)
            {
                continue;
            }

            int adjacentMines = 0;
            for (int dy = y - 1; dy <= y + 1; dy++)
            {
                for (int dx = x - 1; dx <= x + 1; dx++)
                {
                    if (!board.isInBounds(dx, dy) || (dx == x && dy == y))
                    {
                        continue;
                    }

                    for (size_t i = 0; i < unknownCells.size(); i++)
                    {
                        if (unknownCells.at(i).x == dx && unknownCells.at(i).y == dy)
                        {
                            if (config.at(i))
                            {
                                adjacentMines++;
                            }

                            break;
                        }
                    }
                }
            }

            if (adjacentMines != cell.adjacentMines)
            {
                std::cout << "Cell " << cell.x << ", " << cell.y << " : Expected " << cell.adjacentMines << " but got " << adjacentMines;
                std::cout << ". Config: ";
                for (bool flag : config)
                {
                    std::cout << flag;
                }
                std::cout << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

void Solver::generateConfigurations(const std::vector<Cell>& unknownCells, std::vector<bool>& config, int index,
                                    int& totalValidConfigs, std::vector<long double>& configsMineCounts)
{
    if (index == unknownCells.size())
    {
        if (!isConfigurationValid(unknownCells, config))
        {
            return;
        }

        totalValidConfigs++;
        int mineCount = 0;
        
        for (size_t i = 0; i < unknownCells.size(); i++)
        {
            Cell& cell = board.getCell(unknownCells.at(i).x, unknownCells.at(i).y);
            cell.totalConfigs++;

            if (config[i])
            {
                cell.flagConfigs++;
                mineCount++;
            }
        }

        configsMineCounts[mineCount]++;
        
        return;
    }
    
    // Try cell as safe (no flag) then mine (flag)
    for (size_t value = 0; value <= 1; value++)
    {
        config.at(index) = value == 1;
        if (isConfigAssignementValid(unknownCells, config, index))
        {
            generateConfigurations(unknownCells, config, index + 1, totalValidConfigs, configsMineCounts);
        }
    }
}

void Solver::solveBoard()
{
    for (Cell& cell : board.grid)
    {
        cell.isSolvedFlag = false;
        cell.flagConfigs = 0;
        cell.totalConfigs = 0;
        cell.mineProbability = 0.0f;
    }

    std::vector<Cell> frontierCells = getFrontierCells();
    std::vector<Cell> hiddenCells = getHiddenCells();
    std::cout << "Unknown cells: " << frontierCells.size() << std::endl;

    std::vector<bool> config(frontierCells.size(), false);
    std::vector<long double> configsMineCounts(frontierCells.size() + 1, 0);
    
    int totalValidConfigs = 0;
    generateConfigurations(frontierCells, config, 0, totalValidConfigs, configsMineCounts);

    std::cout << "Valid configurations: " << totalValidConfigs << std::endl;

    if (totalValidConfigs == 0)
    {
        return;
    }

    for (const Cell& frontierCell : frontierCells)
    {
        Cell& cell = board.getCell(frontierCell.x, frontierCell.y);
        cell.mineProbability = static_cast<float>(cell.flagConfigs) / static_cast<float>(cell.totalConfigs);
        std::cout << "Cell " << cell.x << ", " << cell.y << " : " << cell.flagConfigs << " flag configs / " << cell.totalConfigs << " total configs = " << cell.mineProbability << std::endl;
    }

    long double totalBoardConfigs = 0;
    long double mineBoardConfigs = 0;
    int frontierCellsSize = frontierCells.size();
    int hiddenCellsSize = hiddenCells.size();

    for (int k = 0; k <= frontierCellsSize; k++)
    {
        int remainingMines = board.minesCount - k;

        if (remainingMines >= 0 && remainingMines <= hiddenCellsSize)
        {
            totalBoardConfigs += configsMineCounts[k] * choose(hiddenCellsSize, remainingMines);
        }

        if (remainingMines >= 1 && remainingMines <= hiddenCellsSize)
        {
            mineBoardConfigs += configsMineCounts[k] * choose(hiddenCellsSize - 1, remainingMines - 1);
        }
    }

    if (totalBoardConfigs <= 0)
    {
        return;
    }

    long double hiddenCellProbablity = mineBoardConfigs / totalBoardConfigs;
    std::cout << "Hidden cells probability = " << mineBoardConfigs << " mine board configs / " << totalBoardConfigs
        << " total board configs = " << hiddenCellProbablity << std::endl;
    for (const Cell& hiddenCell : hiddenCells)
    {
        Cell &cell = board.getCell(hiddenCell.x, hiddenCell.y);
        cell.mineProbability = hiddenCellProbablity;
    }
}

void Solver::displaySolverResults()
{
    for (int index = 0; index < board.totalCells; index++)
    {
        Cell &cell = board.grid[index];
        if (!cell.isRevealed || cell.visualState == EXPLODED)
        {
            Draw::drawMineProbability(index % board.width, index / board.width);
        }
    }
}
