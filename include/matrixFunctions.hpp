#ifndef MATRIXFUNCTIONS_HPP_INCLUDED
#define MATRIXFUNCTIONS_HPP_INCLUDED

#include <vector>

// Создание нулевой матрицы Y x X
std::vector<std::vector<double>> createFilledVector(int y, int x);

// Вырезка из matrix прямоуголника AxB с координат y0, x0 (Добавить защиту!!!)
std::vector<std::vector<double>> matrixSlicer(std::vector<std::vector<double>> matrix, unsigned y0, unsigned x0, unsigned a, unsigned b);

// a * b = c (поэлементное произведение)
std::vector<std::vector<double>> dot(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b);

// a + b = c (поэлементное сложение)
std::vector<std::vector<double>> sumElements(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b);

// Сумма всех элементов
double elementsSum(std::vector<std::vector<double>> a);

// Получение расширенной матрицы
std::vector<std::vector<double>> matrixExpansion(std::vector<std::vector<double>> matrix, unsigned padding);

#endif // MATRIXFUNCTIONS_HPP_INCLUDED
