#include "../include/board.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::cout << "Not enough arguments !" << std::endl << "Usage: ./game <width> <height> <mines>" << std::endl;
        return 1;
    }

    int width = std::stoi(argv[1]);
    int height = std::stoi(argv[2]);
    int mines = std::stoi(argv[3]);

    if (mines > width * height)
    {
        std::cout << "Board is not big enough to place all the mines !" << std::endl;
        return 1;
    }
    
    Board board(width, height, mines);
    board.placeAllMines();
    board.updateAllCellAdjacencies();
    board.printBoard();
    return 0;
}
