#include <cstddef>
#include <vector>
#include "board.hpp"

class Solver
{
    public:
        Board &board;
        float minMineProbability = 0;
                
        Solver(Board& board);

        static long double factorial(int n);
        static long double choose(int n, int k);
        
        std::vector<Cell> getFrontierCells(); // Returns all unrevealed cells neighboring revealed cells (numbers)
        std::vector<Cell> getHiddenCells(); // Returns all unrevealed cells not neighboring revealed cells (numbers)
        
        bool isConfigAssignementValid(const std::vector<Cell>& unknownCells, const std::vector<bool>& config, int index);
        bool isConfigurationValid(const std::vector<Cell>& unknownCells, const std::vector<bool>& config);
        void generateConfigurations(const std::vector<Cell>& unknownCells, std::vector<bool>& config, int index,
                                    int& totalValidConfigs, std::vector<long double>& configsMineCounts);
        void solveBoard();      
        void displaySolverResults();
};
