#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include <vector>

#include "../include/solver.hpp"

template <typename T>
void vector_eq(const std::vector<T> &expected, const std::vector<T> &actual)
{
    cr_assert_eq(expected.size(), actual.size());
    for (int i = 0; i < expected.size(); i++)
    {
        cr_assert_eq(expected.at(i), actual.at(i), "Expected %d but got %d", expected.at(i), actual.at(i));
    }
}

Test(solver, getAllConfigs_1_0)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(1, 0);
    vector_eq({0b0}, actualConfigs);
}
Test(solver, getAllConfigs_1_1)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(1, 1);
    vector_eq({0b1}, actualConfigs);
}
Test(solver, getAllConfigs_2_1)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(2, 1);
    vector_eq({0b10, 0b01}, actualConfigs);
}
Test(solver, getAllConfigs_2_2)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(2, 2);
    vector_eq({0b11}, actualConfigs);
}
Test(solver, getAllConfigs_3_2)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(3, 2);
    vector_eq({0b110, 0b101, 0b011}, actualConfigs);
}

Test(solver, solveCell_easy)
{
    Board *board = new Board(3, 2, 1);
    board->getCell(1, 1).isMine = true;
    board->updateAllCellAdjacencies();
    board->getCell(0, 0).isRevealed = true;
    board->getCell(1, 0).isRevealed = true;
    Solver *solver = new Solver(*board);
    Cell &solvedCell = board->getCell(0, 0);
    cr_assert_eq(1, solvedCell.adjacentMines, "Expected 1 adjacent mine but got %d", solvedCell.adjacentMines);

    std::vector<Cell> adjacentEmptyCells = board->getAdjacentEmptyCells(solvedCell.x, solvedCell.y);
    cr_assert_eq(2, adjacentEmptyCells.size());
    std::vector<int> configs = Solver::getAllConfigs(2, 1);
    vector_eq({0b10, 0b01}, configs);
    
    solver->solveCell(0, 0);
    Cell &cell = board->getCell(0, 1);
    cr_assert_eq(1, cell.flagConfigs, "Expected 1 flag configs but got %d", cell.flagConfigs);
    cr_assert_eq(2, cell.totalConfigs, "Expected 2 total configs but got %d", cell.totalConfigs);
    cr_assert_float_eq(0.5, cell.mineProbability, 0.01f, "Expected 0.5 mine probability but got %.2f", cell.mineProbability);
    delete solver;
    delete board;
}

Test(solver, solveCell_hard)
{
    Board *board = new Board(3, 2, 1);
    board->getCell(1, 1).isMine = true;
    board->updateAllCellAdjacencies();
    board->getCell(0, 0).isRevealed = true;
    board->getCell(1, 0).isRevealed = true;
    board->getCell(2, 0).isRevealed = true;
    Solver *solver = new Solver(*board);
    Cell &solvedCell = board->getCell(0, 0);
    cr_assert_eq(1, solvedCell.adjacentMines, "Expected 1 adjacent mine but got %d", solvedCell.adjacentMines);

    std::vector<Cell> adjacentEmptyCells = board->getAdjacentEmptyCells(solvedCell.x, solvedCell.y);
    cr_assert_eq(2, adjacentEmptyCells.size());
    std::vector<int> configs = Solver::getAllConfigs(2, 1);
    vector_eq({0b10, 0b01}, configs);
    
    solver->solveCell(0, 0);
    Cell &cell = board->getCell(0, 1);
    cr_assert_eq(0, cell.flagConfigs, "Expected 0 flag configs but got %d", cell.flagConfigs);
    cr_assert_eq(1, cell.totalConfigs, "Expected 1 total configs but got %d", cell.totalConfigs);
    cr_assert_float_eq(0, cell.mineProbability, 0.01f, "Expected 0.5 mine probability but got %.2f", cell.mineProbability);
    delete solver;
    delete board;
}
