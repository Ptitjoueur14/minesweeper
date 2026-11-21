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

Test(solver, getConfigs_1_0)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(1, 0);
    vector_eq({0}, actualConfigs);
}
Test(solver, getConfigs_1_1)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(1, 1);
    vector_eq({1}, actualConfigs);
}
Test(solver, getConfigs_2_1)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(2, 1);
    vector_eq({2, 1}, actualConfigs);
}
Test(solver, getConfigs_2_2)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(2, 2);
    vector_eq({3}, actualConfigs);
}
Test(solver, getConfigs_3_2)
{
    std::vector<int> actualConfigs = Solver::getAllConfigs(3, 2);
    vector_eq({6, 5, 3}, actualConfigs);
}
