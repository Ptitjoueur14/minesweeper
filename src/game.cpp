#include "../include/board.hpp"

int main()
{
    Board board(9, 9, 10);
    board.placeAllMines();
    board.printBoard();
    return 0;
}
