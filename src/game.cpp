#include "../include/board.hpp"
#include <iostream>
#include <string>
#include "../include/ui.hpp"

void printGameUsage()
{
    std::cout << "Couldn't create a board !" << std::endl;
    std::cout << "Usage: ./game" << std::endl;
    std::cout << "Usage: ./game <beg/int/exp>" << std::endl;
    std::cout << "Usage: ./game <width> <height> <mines>" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc == 3)
    {
        printGameUsage();
        return 1;
    }

    int width;
    int height;
    int mines;

    if (argc == 1)
    {
        width = 9;
        height = 9;
        mines = 10;
    }
    else if (argc == 2)
    {
        std::string gameDifficulty = argv[1];
        if (gameDifficulty == "beg")
        {
            width = 9;
            height = 9;
            mines = 10;
        }
        else if (gameDifficulty == "int")
        {
            width = 16;
            height = 16;
            mines = 40;
        }
        else if (gameDifficulty == "exp")
        {
            width = 30;
            height = 16;
            mines = 99;
        }
        else
        {
            printGameUsage();
            return 1;
        }
    }
    else
    {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
        mines = std::stoi(argv[3]);
    }
    
    if (mines > width * height)
    {
        std::cout << "Board is not big enough to place all the mines ! Got << "
            << mines << " mines but only " << width * height << " total cells" << std::endl;
        return 1;
    }
    
    Board *board = new Board(width, height, mines);
    GameUI::board = board;

    create_window();
    
    return 0;
}
