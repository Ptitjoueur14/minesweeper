#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>

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

        Board(int w, int h, int mines);
        
        Cell& getCell(int x, int y);

        void printBoard();

        int getRandomNumber(int max);
        void placeRandomMine();
        void placeAllMines();

        bool isInBounds(int w, int h);
        void updateCellAdjacency(int w, int h);
        void updateAllCellAdjacencies();
};

#endif
