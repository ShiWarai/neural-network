#ifndef NEURAL_IO_HPP_INCLUDED
#define NEURAL_IO_HPP_INCLUDED

#include <vector>
#include <fstream>

// Запись матрицы в файл
void writeMatrixInFile(std::ofstream& fout, std::vector<std::vector<double>> matrix);

// Чтение матрицы из файла
std::vector<std::vector<double>> readMatrixFromFile(std::ifstream& fin);

#endif // NEURAL_IO_HPP_INCLUDED
