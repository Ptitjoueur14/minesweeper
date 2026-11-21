#ifndef BOARD_HPP
#define BOARD_HPP

#include <random>
#include <vector>
#include "cell.hpp"

class Board
{
public:
    int width;
    int height;
    int minesCount;
    std::vector<Cell> grid; // The grid representing the board cells

    int totalCells;
    int remainingMines;

    // Game stats
    float solvingTime; // The time in seconds taken to solve the board
    int solved3BV; // The solved 3BV of the board, excluding unrevealed cells
    int total3BV; // The total 3BV of the board, including unrevealed cells
    float solved3BVPerSecond; // The solved 3BV/s (solved3BV / solvingTime)
    int totalClicks;
    int leftClicks;
    int rightClicks;
    int chordClicks;
    int efficiency; // The efficiency of the board (solved3BV / totalClicks *
                    // 100)

    Board(int w, int h, int mines);

    Cell &getCell(int w, int h);

    void printBoard();

    bool isInBounds(int w, int h);

    int getRandomNumber(int max);
    void placeRandomMine(int firstClickX, int firstClickY);
    void placeAllMines(int firstClickX, int firstClickY);

    void updateCellAdjacency(int w, int h);
    void updateAllCellAdjacencies();

    void expandConnectedZeroRegion(std::vector<bool> &visited, int cellX,
                                   int cellY);
    int countZeroRegions(bool isSolved);
    bool isAdjacentToZero(int cellX, int cellY);
    int countIsolatedNumbers(bool isSolved);
    int calculate3BV(bool isSolved); // Calculate the 3BV of the board

    std::vector<struct Cell> getAdjacentEmptyCells(int cellX, int cellY);

private:
    std::mt19937 rng;
};

#endif
