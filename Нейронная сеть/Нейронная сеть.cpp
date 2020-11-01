#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <time.h>
#include "BMP_reading.h"

using namespace std;
using namespace BMP;

// Максимум из 4 переменных
double max4(double a, double b, double c, double d)
{
	a = max(a, b);
	a = max(a, c);
	a = max(c, d);
	return a;
}

// Создание нулевой матрицы Y x X
vector<vector<double>> createFilledVector(int y, int x) {
	vector<vector<double>> c;

	for (int y1 = 0; y1 < y; y1++) {
		c.push_back(vector<double> {});
		for (int x1 = 0; x1 < x; x1++)
			c[y1].push_back(0);
	}

	return c;
}

// a * b = c (поэлементное произведение)
vector<vector<double>> dot(vector<vector<double>> a, vector<vector<double>> b) {

	if ((a.size() != b.size()) && (a[0].size() != b[0].size())) {
		cout << "Размеры массивов не совпадают" << endl;
		exit(-1);
	}

	vector<vector<double>> c = createFilledVector(a.size(), a[0].size());

	for (int y1 = 0; y1 < a.size(); y1++) {
		for (int x1 = 0; x1 < a[0].size(); x1++) {
			c[y1][x1] = a[y1][x1] * b[y1][x1];
		}
	}

	return c;
}

// a + b = c (поэлементное сложение)
vector<vector<double>> sum(vector<vector<double>> a, vector<vector<double>> b) {

	if ((a.size() != b.size()) && (a[0].size() != b[0].size())) {
		cout << "Размеры массивов не совпадают" << endl;
		exit(-1);
	}

	vector<vector<double>> c = createFilledVector(a.size(), a[0].size());

	for (int y1 = 0; y1 < a.size(); y1++) {
		for (int x1 = 0; x1 < a[0].size(); x1++) {
			c[y1][x1] = a[y1][x1] + b[y1][x1];
		}
	}

	return c;
}

double elementsSum(vector<vector<double>> a) {
	double sum = 0;
	for (int y1 = 0; y1 < a.size(); y1++) {
		for (int x1 = 0; x1 < a[0].size(); x1++) {
			sum += a[y1][x1];
		}
	}

	return sum;
}

// Поэлементный максимум
vector<vector<double>> reluFunction(vector<vector<double>> a) {

	for (int y1 = 0; y1 < a.size(); y1++) {
		for (int x1 = 0; x1 < a[0].size(); x1++) {
			a[y1][x1] = max(a[y1][x1], 0);
		}
	}

	return a;
}

// Получение расширенной матрицы
vector<vector<double>> matrixExpansion(vector<vector<double>> matrix, unsigned padding) {

	auto new_matrix = createFilledVector(matrix.size() + padding, matrix[0].size() + padding);
	int max_panding_1 = (int)ceil((double)padding / 2); // Северо-запад
	int max_panding_2 = (int)floor((double)padding / 2); //Юго-восток

	int currentX, currentY;

	// Заполнение расширения
	for (int padd_ = 1; padd_ <= padding; padd_++) {

		// Вычисляем отступ
		int panding = (int)ceil((double)padd_ / 2) - 1;

		// Для северо-запада
		if (padd_ % 2 == 1)
		{
			// Север
			
			currentY = 0 + panding;

			for (int x = 0; x < new_matrix[0].size(); x++) {
				if (x < max_panding_1) {
					new_matrix[currentY][x] = matrix[0][0];
				}
				else if (x > new_matrix[0].size() - max_panding_2 - 1) {
					new_matrix[currentY][x] = matrix[0][matrix[0].size() - 1];
				}
				else {
					new_matrix[currentY][x] =
						matrix[0][x - max_panding_1];
				}
			}

			// Запад

			currentX = 0 + panding;

			for (int y = 0; y < new_matrix.size(); y++) {
				if (y < max_panding_1) {
					new_matrix[y][currentX] = matrix[0][0];
				}
				else if (y > new_matrix.size() - max_panding_2 - 1) {
					new_matrix[y][currentX] = matrix[matrix.size() - 1][0];
				}
				else {
					new_matrix[y][currentX] =
						matrix[y - max_panding_1][0];
				}
			}

		}
		// Для юго-востока
		else
		{
			// Юг
			currentY = new_matrix.size() - 1 - panding;

			for (int x = 0; x < new_matrix[0].size(); x++) {
				if (x < max_panding_1) {
					new_matrix[currentY][x] = matrix[matrix.size() - 1][0];
				}
				else if (x > new_matrix[0].size() - max_panding_2 - 1) {
					new_matrix[currentY][x] = matrix[matrix[0].size() - 1][matrix[0].size() - 1];
				}
				else {
					new_matrix[currentY][x] =
						matrix[matrix[0].size() - 1][x - max_panding_1];
				}
			}

			// Восток
			currentX = new_matrix[0].size() - 1 - panding;

			for (int y = 0; y < new_matrix.size(); y++) {
				if (y < max_panding_1) {
					new_matrix[y][currentX] = matrix[0][matrix[0].size() - 1];
				}
				else if (y > new_matrix.size() - max_panding_2 - 1) {
					new_matrix[y][currentX] = matrix[matrix[0].size() - 1][matrix[0].size() - 1];
				}
				else {
					new_matrix[y][currentX] =
						matrix[y - max_panding_1][matrix[0].size() - 1];
				}
			}
		}

	}

	// Заполнение центра
	for (int y = 0; y < matrix.size(); y++) {
		for (int x = 0; x < matrix[0].size(); x++) {
			new_matrix[y + max_panding_1][x + max_panding_1] = matrix[y][x];
		}
	}

	return new_matrix;

}

// Вырезка из matrix прямоуголника AxB с координат y0, x0 (Добавить защиту!!!)
vector<vector<double>> matrixSlicer(vector<vector<double>> matrix, unsigned y0, unsigned x0, unsigned a, unsigned b) { 
	auto processed_matrix = createFilledVector(a, b);

	for (int y = 0; y < a; y++) {
		for (int x = 0; x < b; x++) {
			processed_matrix[y][x] = matrix[y0+y][x0+x];
		}
	}

	return processed_matrix;
}

// Главная функция прямого распространения
vector<vector<double>> directDistributionFunc(vector<vector<vector<double>>> pic, vector<vector<vector<double>>> core) {

	if (core.size() != pic.size())
		exit(0);

	unsigned y_size = pic[0].size();
	unsigned x_size = pic[0][0].size();


	auto processed_pic = createFilledVector(y_size, x_size);

	int dimensions = pic.size();
	int panding_ = (int)ceil((double)(core[0].size() - 1) / 2); // Определяем начало гл. массива

	vector<vector<double>> flat;
	for (int dim = 0; dim < dimensions; dim++) {

		// Работа с плоскостью
		flat = matrixExpansion(pic[dim], core[0].size() - 1); // Получаем расширенную матрицу плоскости

		for (int y = 0; y < y_size; y++) {
			for (int x = 0; x < x_size; x++){

				auto slice = matrixSlicer(flat, panding_ + y - (core[dim].size() - 1), panding_ + x - (core[dim][0].size() - 1), core[dim].size(), core[dim][0].size()); // Получаем кусок матрицы
				slice = dot(slice, core[dim]); // Находим произведение

				processed_pic[y][x] += elementsSum(slice); // Складываем слои
			}
		}
	}

	// Выводим с использованием relu()
	return reluFunction(processed_pic);
}

// Свёртка к максимальному
vector<vector<double>> max_pooling(vector<vector<double>> a) {
	vector<vector<double>> finalBuffer;
	int currentSize = a.size();

	for (int y = 0; y < currentSize; y += 2) {
		vector<double> buffer;
		for (int x = 0; x < currentSize; x += 2) {

			double c = max4(a[y][x], a[y][x + 1], a[y + 1][x], a[y + 1][x + 1]);

			buffer.push_back(c);

			a[y][x] = 0;
			a[y][x + 1] = 0;
			a[y + 1][x] = 0;
			a[y + 1][x + 1] = 0;
		}
		finalBuffer.push_back(buffer);
	}

	/*
	for (int y = 0; y < currentSize / 2; y++) {
		for (int x = 0; x < currentSize / 2; x++) {
			a[y][x] = finalBuffer[y][x];
			cout << setw(6) << setprecision(4) << a[y][x] << " ";
		}
		cout << endl;
	} */

	return finalBuffer;
}

// Подсчёт вероятности цифр от 0 до 9 в процентах
vector<double> softmax(vector<vector<double>> a) {
	double sum = 0;
	vector<double> c;

	for (int i = 0; i < a.size(); i++)
		sum += a[i][0];

	for (int i = 0; i < a.size(); i++)
		a[i][0] /= sum;

	for (int i = 0; i < a.size(); i++)
		c.push_back(a[i][0]);

	return c;
}

vector<double> flatten(vector<vector<double>> a) {
	vector<double> b;
	for (int y = 0; y < a.size(); y++) {
		for (int x = 0; x < a.size(); x++)
			b.push_back(a[y][x]);
	}

	return b;
}

// Первое заполнение ядер
vector<vector<vector<double>>> generationCore(unsigned CORE_SIZE, unsigned DEPTH) {
	vector<vector<vector<double>>> core;

	for (int i = 0; i < DEPTH; i++)
	{
		core.push_back(vector<vector<double>> {});

		core[i] = createFilledVector(CORE_SIZE, CORE_SIZE);

		for (int y1 = 0; y1 < CORE_SIZE; y1++) {
			for (int x1 = 0; x1 < CORE_SIZE; x1++) {
				core[i][y1][x1] = 1 - ((double)rand() /(RAND_MAX+1)) * 2;
			}
		}
	}

	return core;
}

// Программа
int main()
{
	/*
	vector<vector<double>> a = { {1,4,2},{32,2,0},{5,4,7} };
	auto b = flatten(a);

	for (int y1 = 0; y1 < b.size(); y1++) {
		cout << setw(3) << b[y1];
	}
	*/

	const unsigned int PICTURE_SIZE = 16;
	srand(time(NULL));

	setlocale(LC_ALL, "");

	const wstring PATH = L"C:/Digits/";
	const string PATH_S = "C:/Digits/";

	vector<vector<string>> trainingFiles;

	WIN32_FIND_DATA FindFileData;
	HANDLE handleFiles = FindFirstFileW((PATH + L"*").c_str(), &FindFileData);

	if (handleFiles != INVALID_HANDLE_VALUE)
	{
		while (true) {
			if (FindNextFileW(handleFiles, &FindFileData) != NULL)
			{
				wstring ws(FindFileData.cFileName);
				string fileName(ws.begin(), ws.end());
				string digit = fileName.substr(0, fileName.find("."));

				trainingFiles.push_back(vector<string> {fileName, digit});
			}
			else
			{
				break;
			}
		}
	}

	trainingFiles.erase(trainingFiles.begin()); // Удаляем скрытый файл ".."
	FindClose(handleFiles);

	cout << "\n\n";

	// Вывод названий и цифр
	for (int i = 0; i < trainingFiles.size(); i++)
		cout << "File name:" << trainingFiles[i][0] << "\nDigit:" << trainingFiles[i][1] << endl;

	for (int k = 0; k < trainingFiles.size(); k++) {

		BMP_BW image(trainingFiles[k][1], (string)(PATH_S + trainingFiles[k][0]), false);

		// Тестовый вывод

		/*
		for (int y = PICTURE_SIZE - 1; y >= 0; y--) {
			for (int x = 0; x < PICTURE_SIZE; x++)
				cout << setw(3) << setprecision(3)<< image[y][x] << " ";
			cout << endl;
		}
		*/

		cout << endl;
	}

	// Создание ядер
	vector<vector<vector<vector<vector<double>>>>> cores;
	// Набор ядер для слоя (ядро определённого слоя (глубина ядра (ширина и высота)) )


	const int epochs = 1;

	vector<double> delta;
	double prediction;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= epochs; epoch++) {
		for (int fileNum = 0; fileNum < 1; fileNum++) { // trainingFiles.size()

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);
			cout << "(" << epoch << ", " << trainingFiles[fileNum][0] << ") :" << endl;

			// Слой 1
			vector<vector<vector<vector<double>>>> cores_set;
			int output_dim = 32;

			// Генерация или чтение набора ядер
			if (epoch == 1 && fileNum == 0) {

				for (int i = 0; i < output_dim; i++) {
					cores_set.push_back(vector<vector<vector<double>>> {});
					cores_set[i] = generationCore(2, 1);
				}

				cores.push_back(cores_set);
			}
			else {
				cores_set = cores[0];
			}

			vector<vector<vector<double>>> layers1;

			vector<vector<double>> new_matrix;
			for (int i = 0; i < output_dim; i++) {
				auto core = cores_set[i];
				new_matrix = directDistributionFunc(vector<vector<vector<double>>> {image.getImage()}, core);

				for (int y = 0; y < new_matrix.size(); y++) {
					for (int x= 0; x < new_matrix[0].size(); x++) {
						cout << setw(5) << setprecision(3)<< new_matrix[y][x];
					}
					cout << endl;
				}
				cout << endl;

				// Max_pooling 2x2
				new_matrix = max_pooling(new_matrix);

				// Добавляем уже сжатый слой
				layers1.push_back(new_matrix); 
			}


			cout << "2 LAYER" << endl;

			// Слой 2
			output_dim = 16;
			cores_set.clear();

			// Генерация или чтение набора ядер
			if (epoch == 1 && fileNum == 0) {

				for (int i = 0; i < output_dim; i++) {
					cores_set.push_back(vector<vector<vector<double>>> {});
					cores_set[i] = generationCore(2, 32);
				}

				cores.push_back(cores_set);
			}
			else {
				cores_set = cores[1];
			}

			vector<vector<vector<double>>> layers2;

			new_matrix.clear();
			for (int i = 0; i < output_dim; i++) {
				auto core = cores_set[i];
				new_matrix = directDistributionFunc(layers1, core);

				for (int y = 0; y < new_matrix.size(); y++) {
					for (int x = 0; x < new_matrix[0].size(); x++) {
						cout << setw(5) << setprecision(3)<< new_matrix[y][x];
					}
					cout << endl;
				}
				cout << endl;

				// Max_pooling 2x2
				new_matrix = max_pooling(new_matrix);

				layers2.push_back(new_matrix);
			}


			delta.clear();
		}



	}

	return 0;
}