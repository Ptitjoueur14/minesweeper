#ifndef CELL_HPP
#define CELL_HPP

enum CellVisualState
{
    HIDDEN,
    REVEALED,
    FLAGGED,
    HOVERED,
    PRESSED,
    EXPLODED,
    MISPLACED_FLAG,
};

class Cell
{
public:
    int x;
    int y;
    bool isMine = false; // If the cell contains a mine
    bool isRevealed = false; // If the cell is revealed
    bool isFlagged = false; // If the cell is flagged
    bool isHovered = false; // If the cell is hovered (click or chord to check
                            // unrevaled and unflagged adjacent cells)
    bool isPressed = false; // If the cell is pressed (left click down)
    int adjacentMines =
        0; // The number of mines in a 3x3 area neighboring the cell
    enum CellVisualState visualState;

    bool isSolvedFlag;
    int flagConfigs;
    int totalConfigs;
    float mineProbability; // The probability of the cell being a mine, between 0 and 1

    Cell(int cellX, int cellY);
    void updateCellVisualState(bool isGameFinished, bool isGameWon);
};

#endif /* ! CELL_HPP */
