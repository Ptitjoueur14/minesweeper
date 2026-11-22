#include <vector>
#include "board.hpp"

class Solver
{
    public:
        Board &board;
                
        Solver(Board& board);
        std::vector<Cell> getUnknownCells();
        bool isConfigAssignementValid(const std::vector<Cell>& unknownCells, const std::vector<bool>& config, int index);
        bool isConfigurationValid(const std::vector<Cell>& unknownCells, const std::vector<bool>& config);
        void generateConfigurations(const std::vector<Cell>& unknownCells, std::vector<bool>& config, int index, int& totalValidConfigs);
        void solveBoard();      
        void displaySolverResults();
};
