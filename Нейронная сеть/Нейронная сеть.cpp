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

#include "functions.h"
#include "matrixFunctions.h"
#include "neuralNetwork.h"
#include "neural_IO.h"
#include "testingArea.h"

vector<vector<double>> generationBias(int a, int b, double koef = 10);
vector<vector<vector<double>>> Dense(vector<vector<vector<double>>> input, vector<vector<vector<vector<double>>>> cores_set, vector<vector<vector<double>>>  biases_set, unsigned outputLayers, bool do_max_pooling = true);

// Программа
int main()
{
	srand(abs(rand() - time(NULL)) * 100);
	setlocale(LC_ALL, "ru");
	// cout.setf(ios::fixed);

	ifstream finCores, finBiases;
	ofstream foutCores, foutBiases;
	string pathCores = "cores.dat", pathBiases = "biases.dat";


	// Тестовый полигон
	/*testNet();
	vector<vector<double>> matrix;
	vector<double> v;
	v.push_back(1.2);
	v.push_back(1.7);
	v.push_back(2.5);
	v.push_back(9.5);
	v.push_back(5.7);
	matrix.push_back(v);
	v.clear();
	v.push_back(2.4);
	v.push_back(5.9);
	v.push_back(3.8);
	v.push_back(8.6);
	v.push_back(5.0);
	matrix.push_back(v);
	writeMatrixInFile(matrix,"cores.txt");
	writeMatrixInFile(matrix,"cores.txt");
	matrix = readMatrixFromFile("cores.txt");
	consoleOutMatrix(matrix);
	exit(1);*/

	
	static vector<vector<string>> trainingFiles;

	const unsigned int PICTURE_SIZE = 16;
	const wstring PATH = L"C:/Digits/";
	const string PATH_S = "C:/Digits/";
	//Чтение файлов
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE handleFiles = FindFirstFileW((PATH + L"*").c_str(), &FindFileData);

		if (handleFiles != INVALID_HANDLE_VALUE)
		{
			while (true)
			{
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

		// Проверка чтения изображения
		for (int k = 0; k < trainingFiles.size(); k++)
		{

			BMP_BW image(trainingFiles[k][1], (string)(PATH_S + trainingFiles[k][0]), true);

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
	}

	// Инициализация ядер и смещения
	vector<vector<vector<vector<vector<double>>>>> cores;
	vector<vector<vector<vector<double>>>> biases;


	cout << "Считать ядра и смещения с файла? (0 - нет, 1 - да)\n";
	int check;
	cin >> check;
	bool needToGenerate;

	switch (check)
	{
	case 0:
		needToGenerate = true;
		break;
	case 1:
		needToGenerate = false;
		break;
	default:
		cout << "Вы ввели недопустимое число";
		return -4;
		break;
	}

	if (!needToGenerate) {
		finCores.open(pathCores);
		finBiases.open(pathBiases);
		if (!finCores.is_open() || !finBiases.is_open())
		{
			cout << "Ошибка открытия файла для чтения";
			return -5;
		}
	}


	const int EPOCHS = 1;

	vector<double> delta;
	double prediction;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= EPOCHS; epoch++) {

		for (int fileNum = 0; fileNum < trainingFiles.size(); fileNum++) {

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);
			cout << "(" << epoch << ", " << trainingFiles[fileNum][0] << ") :" << endl;

			// Прямой ход
			
			int output_dim = 16;
			vector<vector<vector<vector<double>>>> cores_set;
			vector<vector<vector<double>>> biases_set;

			// Слой 1

			cout << "1 LAYER" << endl;
			unsigned layer_num = 1;

			// Генерация или чтение набора ядер и смещения
			if (epoch == 1 && fileNum == 0)
			{
				int DEPTH = 1, CORE_SIZE = 2;

				for (int i = 0; i < output_dim; i++)
				{
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});
						cores_set[i] = generationCore(DEPTH, CORE_SIZE);

						biases_set.push_back(generationBias(image.getHeight(), image.getWidth()));
					}
					else
					{						
						cores_set.push_back(vector<vector<vector<double>>> {});

						for (int j = 0; j < DEPTH; j++)
						{
							cores_set[i].push_back(vector<vector<double>> {});

							cores_set[i][j] = readMatrixFromFile(finCores);
						}

						biases_set.push_back(readMatrixFromFile(finBiases));
					}
					
				}

				cores.push_back(cores_set);
				biases.push_back(biases_set);
			}
			else
			{
				cores_set = cores[layer_num - 1];
				biases_set = biases[layer_num - 1];
			}

			vector<vector<vector<double>>> layer1 = Dense(vector<vector<vector<double>>> {image.getImage()}, cores_set, biases_set, output_dim);


			//for(int i = 0; i < layer1.size();i++)
			//	consoleOutMatrix(layer1[i]);


			// Слой 2
			cout << "2 LAYER" << endl;
			layer_num += 1;

			output_dim = 16;

			// Генерация или чтение набора ядер
			cores_set.clear();
			biases_set.clear();
			if (epoch == 1 && fileNum == 0) {

				int DEPTH = layer1.size();

				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						cores_set[i] = generationCore(DEPTH, 2);

						biases_set.push_back(generationBias(layer1.size(), layer1[0].size()));
					}
					else
					{
						cores_set.push_back(vector<vector<vector<double>>> {});


						for (int j = 0; j < DEPTH; j++)
						{
							cores_set[i].push_back(vector<vector<double>> {});

							cores_set[i][j] = readMatrixFromFile(finCores);
						}

						biases_set.push_back(readMatrixFromFile(finBiases));
					}
				}

				cores.push_back(cores_set);
				biases.push_back(biases_set);
			}
			else {
				cores_set = cores[layer_num - 1];
				biases_set = biases[layer_num - 1];
			}

			vector<vector<vector<double>>> layer2 = Dense(layer1, cores_set, biases_set, output_dim);


			//for (int i = 0; i < layer2.size(); i++)
			//	consoleOutMatrix(layer2[i]);



			// Слой 3
			cout << "3 LAYER" << endl;
			layer_num += 1;

			output_dim = 16;

			// Генерация или чтение набора ядер
			cores_set.clear();
			biases_set.clear();
			if (epoch == 1 && fileNum == 0) {
				int DEPTH = layer2.size();

				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						cores_set[i] = generationCore(DEPTH, 2);

						biases_set.push_back(generationBias(layer2.size(), layer2[0].size()));
					}
					else
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						for (int j = 0; j < DEPTH; j++)
						{
							cores_set[i].push_back(vector<vector<double>> {});

							cores_set[i][j] = readMatrixFromFile(finCores);
						}

						biases_set.push_back(readMatrixFromFile(finBiases));
					}
				}

				cores.push_back(cores_set);
				biases.push_back(biases_set);
			}
			else {
				cores_set = cores[layer_num - 1];
				biases_set = biases[layer_num - 1];
			}

			vector<vector<vector<double>>> layer3 = Dense(layer2, cores_set, biases_set, output_dim);


			//for (int i = 0; i < layer3.size(); i++)
			//	consoleOutMatrix(layer3[i]);



			// 4 слой
			cout << "4 LAYER" << endl;
			vector<double> layer4;

			vector<double> new_matrix;
			for (int i = 0; i < layer3.size(); i++) {
				// flatten
				new_matrix = flatten(layer3[i]);

				for (int k = 0; k < new_matrix.size(); k++)
					layer4.push_back(new_matrix[k]);

			}


			//for (int x = 0; x < layer4.size(); x++)
			//	cout << setprecision(6) << layer4[x] << endl;


			// 5 слой (слой выхода)
			// cout << "5 LAYER" << endl;
			layer_num += 1;

			output_dim = 10;
			cores_set.clear();
			biases_set.clear();

			// Генерация или чтение набора ядер
			if (epoch == 1 && fileNum == 0) {

				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						cores_set[i].push_back(vector<vector<double>> { generationWeights(layer4.size()) });
					}
					else
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						int DEPTH = 1;

						for (int j = 0; j < DEPTH; j++)
						{
							cores_set[i].push_back(vector<vector<double>> {});

							cores_set[i][j] = readMatrixFromFile(finCores);
						}
					}
				}

				cores.push_back(cores_set);
			}
			else {
				cores_set = cores[layer_num - 1];
			}

			vector<vector<vector<double>>> layer5;

			for (int i = 0; i < output_dim; i++) {
				double sum = 0;

				for (int k = 0; k < layer4.size(); k++) {
					sum += layer4[k] * cores_set[i][0][0][k];
				}

				layer5.push_back(vector<vector<double>> { {max(0, sum)}}); // ReLu
			}

			// Просчёт вероятностей
			auto result = softmax(layer5);

			cout << endl << "Result:" << endl;
			for (int x = 0; x < result.size(); x++)
				cout << result[x] << endl;
			cout << endl;

			// Итоговое предсказание
			int prediction = 0;

			double maxProc = result[0];
			for (int i = 0; i < result.size(); i++) {
				if (result[i] > maxProc) {
					maxProc = result[i];
					prediction = i;
				}
			}

			cout << "Prediction:" << prediction << endl << endl;

			delta = getDelta(result, stoi(image.getName()));

			cout << "Summary loss:" << getLoss(result, getUnitaryCode(result.size(), stoi(image.getName()))) << endl << endl;

			for (int i = 0; i < delta.size(); i++)
				cout << setprecision(6) << setw(10) << delta[i];
			cout << endl;

			// Обратный ход
			//
			//
			// Обратный ход

			
		}
	}


	if (!needToGenerate)
	{
		finCores.close();
		finBiases.close();
	}
	else {

		// Запись в файл

		foutCores.open(pathCores);
		foutBiases.open(pathBiases);
		if (!foutCores.is_open() || !foutBiases.is_open())
		{
			cout << "Ошибка открытия файла для записи";
			return -6;
		}
		foutCores.clear();
		foutBiases.clear();

		// 1 слой

		int DEPTH = 1;
		int output_dim = 16;
		for (int i = 0; i < output_dim; i++)
		{

			for (int j = 0; j < DEPTH; j++)
			{
				writeMatrixInFile(foutCores, cores[0][i][j]);
			}

			writeMatrixInFile(foutBiases, biases[0][i]);
		}

		// 2 слой
		output_dim = 16;

		DEPTH = 16;

		for (int i = 0; i < output_dim; i++)
		{


			for (int j = 0; j < DEPTH; j++)
			{
				writeMatrixInFile(foutCores, cores[1][i][j]);
			}

			writeMatrixInFile(foutBiases, biases[1][i]);
		}

		// 3 слой

		DEPTH = 16;
		output_dim = 16;

		for (int i = 0; i < output_dim; i++)
		{
			for (int j = 0; j < DEPTH; j++)
			{
				writeMatrixInFile(foutCores, cores[2][i][j]);
			}

			writeMatrixInFile(foutBiases, biases[2][i]);
		}


		// 5 слой

		DEPTH = 1;
		output_dim = 10;
		for (int i = 0; i < output_dim; i++)
		{
			for (int j = 0; j < DEPTH; j++)
			{
				writeMatrixInFile(foutCores, cores[3][i][j]);
			}
		}


		foutCores.close();
		foutBiases.close();
	}

	return 0;
}