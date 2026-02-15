#ifndef NEURALNETWORK_HPP_INCLUDED
#define NEURALNETWORK_HPP_INCLUDED

#include <vector>
#include "matrixFunctions.hpp"
#include "functions.hpp"

// Активационная функция (ReLU)
std::vector<std::vector<double>> reluFunction(std::vector<std::vector<double>> a);

// Макс в пулинге (блок 2x2)
std::vector<std::vector<std::vector<double>>> max_pooling(std::vector<std::vector<double>> a);

// Обратное преобразование max_pooling для окна 2x2
[[nodiscard]] std::vector<std::vector<double>> reverse_max_pooling(std::vector<std::vector<double>> a, std::vector<std::vector<double>> max_poses);

// Нормализация чисел от 0 до 9 в вероятности (softmax)
[[nodiscard]] std::vector<double> softmax(std::vector<std::vector<std::vector<double>>> a);

// Преобразование в вектор (flatten)
[[nodiscard]] std::vector<double> flatten(std::vector<std::vector<double>> a);

// Обратное преобразование (от слоя flatten к тензору, размеры dim1, dim2, dim3)
[[nodiscard]] std::vector<std::vector<std::vector<double>>> reverse_flatten(std::vector<double> a, int dim1, int dim2, int dim3);

// Функция потерь
[[nodiscard]] double getLoss(std::vector<double> y, std::vector<double> solution);

// der_E8 = d(loss(R)) / d(R)
double der_loss(std::vector<std::vector<std::vector<double>>> layerE6, int n, double solution);

// der_E7 = d(softmax(R,A))/d(R)
double der_softmax(std::vector<std::vector<std::vector<double>>> layerE6, int leader_n, int n, double sums);

// der_E6 = d(relu(x))/d(x)
double der_relu(double x);

// der_E4 = d(W*X*E6_x..)/dX
[[nodiscard]] std::vector<double> ders_E4(std::vector<std::vector<double>> W, std::vector<double> E6_x);

// Производные ядра (свёрточные ядра 2x2 и 3x3)
[[nodiscard]] std::vector<std::vector<std::vector<double>>> ders_cores(std::vector<std::vector<double>> input, std::vector<std::vector<std::vector<double>>> ders_E1, unsigned core_size);

// Производная по весу в полносвязном слое
double getLossDerivative2D(std::vector<std::vector<std::vector<double>>> layer, std::vector<std::vector<double>> w, int j, double sum, double sums, int solution);

// Вычисление дельты
std::vector<double> getDelta(std::vector<double> a, int solution);

// Обработка матрицы с ядром и смещением
std::vector<std::vector<double>> getProcessedMatrix(std::vector<std::vector<std::vector<double>>> matrix, std::vector<std::vector<std::vector<double>>> core, std::vector<std::vector<double>> bias);

std::vector<std::vector<double>> getProcessedMatrix(std::vector<std::vector<std::vector<double>>> matrix, std::vector<std::vector<std::vector<double>>> core);

// Полносвязный слой сети (Dense)
std::vector<std::vector<std::vector<double>>> Dense(std::vector<std::vector<std::vector<double>>> input, std::vector<std::vector<std::vector<std::vector<double>>>> cores_set, unsigned outputLayers, std::vector<std::vector<std::vector<double>>> biases_set);

// Генерация ядра свёртки
std::vector<std::vector<std::vector<double>>> generationCore(unsigned DEPTH, unsigned CORE_SIZE);

// Генерация смещения
std::vector<std::vector<double>> generationBias(int a, int b, double koef);

// Генерация весов для полносвязного слоя
std::vector<double> generationWeights(int a);

#endif // NEURALNETWORK_HPP_INCLUDED
