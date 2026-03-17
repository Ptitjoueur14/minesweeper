#include "../include/board.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include "../include/ui.hpp"

int main(int argc, char** argv)
{
    if (argc >= 2 && argc < 4)
    {
        std::cout << "Not enough arguments !" << std::endl << "Usage: ./game <width> <height> <mines>" << std::endl;
        return 1;
    }

    int width;
    int height;
    int mines;

    if (argc < 4)
    {
        width = 9;
        height = 9;
        mines = 10;
    }
    else
    {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
        mines = std::stoi(argv[3]);
    }
    
    if (mines > width * height)
    {
        std::cout << "Board is not big enough to place all the mines !" << std::endl;
        return 1;
    }
    
    Board board(width, height, mines);
    board.placeAllMines();
    board.updateAllCellAdjacencies();
    board.printBoard();

    create_window(board);
    
    return 0;
}
