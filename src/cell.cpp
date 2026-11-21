#include "../include/cell.hpp"

Cell::Cell(int cellX, int cellY) : x(cellX), y(cellY){};

void Cell::updateCellVisualState(bool isGameFinished,
                                             bool isGameWon)
{
    if (!isGameFinished)
    {
        if (!isRevealed && !isHovered && !isFlagged)
        {
            visualState = HIDDEN;
        }
        else if (isHovered && !isPressed)
        {
            visualState = HOVERED;
        }
        else if (isFlagged)
        {
            visualState = FLAGGED;
        }
        else if (isPressed)
        {
            visualState = PRESSED;
        }
        else
        {
            visualState = REVEALED; 
        }
    }

    else if (isGameWon)
    {
        if (isFlagged)
        {
            visualState = FLAGGED;
        }
        else if (isRevealed)
        {
            visualState = REVEALED;
        }
        else
        {
            visualState = HIDDEN;
        }
    }
    else
    {
        if (isMine && isRevealed)
        {
            visualState = EXPLODED;
        }
        else if (isFlagged && !isMine)
        {
            visualState = MISPLACED_FLAG;
        }
        else if (isFlagged)
        {
            visualState = FLAGGED;
        }
        else if (isRevealed)
        {
            visualState = REVEALED;
        }
        else
        {
            visualState = HIDDEN;
        }
    }
}
