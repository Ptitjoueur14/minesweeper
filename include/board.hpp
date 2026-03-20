#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <random>

struct Cell
{
    bool isMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;
};

class Board
{
    public:
        int width;
        int height;
        int minesCount;
        std::vector<Cell> grid;
        
        int totalCells;
        int remainingMines;
        int board3BV; // The calculated 3BV of the board

        Board(int w, int h, int mines);
        
        Cell& getCell(int w, int h);

        void printBoard();

        bool isInBounds(int w, int h);

        int getRandomNumber(int max);
        void placeRandomMine(int firstClickX, int firstClickY);
        void placeAllMines(int firstClickX, int firstClickY);
        
        void updateCellAdjacency(int w, int h);
        void updateAllCellAdjacencies();

        void expandConnectedZeroRegion(std::vector<bool> &visited, int cellX, int cellY);
        int countZeroRegions();
        bool isAdjacentToZero(int cellX, int cellY);
        int countIsolatedNumbers();
        void calculate3BV(); // Calculate the 3BV of the board

    private:
        std::mt19937 rng;
};

#endif
