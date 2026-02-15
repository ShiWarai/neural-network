#include "neural_IO.hpp"
#include "matrixFunctions.hpp"
#include <iostream>
#include <cstdlib>

using namespace std;

// Запись матрицы в файл
void writeMatrixInFile(ofstream& fout, vector<vector<double>> matrix)
{
	int x = matrix[0].size();
	int y = matrix.size();

	fout << x << " " << y << "\n";

	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			fout << matrix[i][j] << " ";
		}
		fout << "\n";
	}
}

// Чтение матрицы из файла
vector<vector<double>> readMatrixFromFile(ifstream& fin)
{
	int x, y;

	fin >> x;
	fin >> y;

	// Защита от некорректного формата (предотвращает OOM)
	const int MAX_DIM = 10000;
	if (x <= 0 || y <= 0 || x > MAX_DIM || y > MAX_DIM) {
		cout << "Ошибка: некорректные размеры матрицы в файле (x=" << x << ", y=" << y << ")" << endl;
		exit(-1);
	}

	vector<vector<double>> matrix = createFilledVector(y, x);

	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			fin >> matrix[i][j];
		}
	}

	return matrix;
}
