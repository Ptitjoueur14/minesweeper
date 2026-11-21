#include <vector>
#include "board.hpp"

class Solver
{
    public:
        Board board;
                
        Solver(Board &board);
        static std::vector<int> getAllConfigs(int cells, int flags);
        bool isMinePlacementValid(int cellX, int cellY);
        void solveCell(int cellX, int cellY);
        void solveBoard();      
        void displaySolverResults();
};
