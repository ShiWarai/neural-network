#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED

#include <vector>

int max4_index(double a, double b, double c, double d);

// Максимум из 4 элементов
[[nodiscard]] std::vector<double> max4(double a, double b, double c, double d);

// Вывод матрицы в консоль
void consoleOutMatrix(std::vector<std::vector<double>> a);

[[nodiscard]] std::vector<double> getUnitaryCode(int vectorSize, int num);

#endif // FUNCTIONS_HPP_INCLUDED
