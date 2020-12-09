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
vector<vector<vector<double>>> Dense(vector<vector<vector<double>>> input, vector<vector<vector<vector<double>>>> cores_set, unsigned outputLayers, vector<vector<vector<double>>>  biases_set = { {{}} });

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
	const double LEARNING_SPEED = 500;
	const wstring PATH = L"C:/training2/";
	const string PATH_S = "C:/training2/";
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
					string digit = fileName.substr(0, fileName.find(char(32)));

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

		std::random_shuffle(trainingFiles.begin(), trainingFiles.end());

		cout << "\n\n";

		/*
		// Вывод названий и цифр
		for (int i = 0; i < trainingFiles.size(); i++)
			cout << "File name:" << trainingFiles[i][0] << "\nDigit:" << trainingFiles[i][1] << endl;
		*/

		/*
		// Проверка чтения изображения
		for (int k = 0; k < trainingFiles.size(); k++)
		{

			BMP_BW image(trainingFiles[k][1], (string)(PATH_S + trainingFiles[k][0]), false);

			// Тестовый вывод

			
			for (int y = PICTURE_SIZE - 1; y >= 0; y--) {
				for (int x = 0; x < PICTURE_SIZE; x++)
					cout << setw(3) << setprecision(3)<< image[y][x] << " ";
				cout << endl;
			}
			

			cout << endl;
		}
		*/
	}

	// Инициализация ядер и смещения
	vector<vector<vector<vector<vector<double>>>>> cores;
	vector<vector<vector<vector<double>>>> biases;


	cout << "Считать ядра и смещения с файла? (0 - нет, 1 - да)\n";
	int check;
	cin >> check;
	bool needToGenerate, straightOnly;


	switch (check)
	{
	case 0:
		needToGenerate = true;
		straightOnly = false;

		break;
	case 1:
		needToGenerate = false;

		cout << "Только прямой ход? (0 - нет, 1 - да)\n"; cin >> check;
		straightOnly = true ? check == 1 : false;

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


	const int EPOCHS = 10;

	vector<double> delta;
	double prediction;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= EPOCHS; epoch++) {

		if (straightOnly && epoch > 1)
			exit(0);

		for (int fileNum = 0; fileNum < trainingFiles.size(); fileNum++) { // trainingFiles.size()

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);
			cout << "(Epoch: " << epoch << ", " << trainingFiles[fileNum][0] << ") :" << endl;

			// Прямой ход
			
			int output_dim = 16;
			vector<vector<vector<vector<double>>>> cores_set;
			vector<vector<vector<double>>> biases_set;

			vector<vector<vector<vector<double>>>> max_poses = 
				{ vector<vector<vector<double>>> {}, vector<vector<vector<double>>> {} };

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

			vector<vector<vector<double>>> layer1 = Dense(vector<vector<vector<double>>> {image.getImage()}, cores_set, output_dim, { {{}} });


			// 2 слой
			cout << "2 LAYER" << endl;
			vector<vector<vector<double>>> layer2;

			for (int i = 0; i < layer1.size(); i++) {
				vector<vector<vector<double>>> buffer = max_pooling(layer1[i]);

				layer2.push_back(buffer[0]);
				max_poses[0].push_back(buffer[1]);
			}


			// 3 слой
			cout << "3 LAYER" << endl;

			vector<vector<vector<double>>> layer3;

			for (int i = 0; i < layer2.size(); i++) {
				vector<vector<vector<double>>> buffer = max_pooling(layer2[i]);

				layer3.push_back(buffer[0]);
				max_poses[1].push_back(buffer[1]);
			}


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
			cout << "5 LAYER" << endl;
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

			/*
			for (int i = 0; i < delta.size(); i++)
				cout << setprecision(6) << setw(10) << delta[i];
			cout << endl;
			*/

			// Обратный ход

			if (!straightOnly) {


				//	5 слой
				layer_num = layer_num;

				vector<vector<double>> weights;
				for (int i = 0; i < cores_set.size(); i++)
					weights.push_back(cores_set[i][0][0]);

				// Расчёт локальный констант

				// sums = sum(relu(a(i,j) * w(i,j)))
				vector<vector<vector<double>>> layer_;
				vector<double> ders_E6;

				double sums = 0;
								
				for (int n = 0; n < weights.size(); n++) {
					double s = 0;

					for (int k = 0; k < weights[0].size(); k++) {
						s += layer4[k] * weights[n][k];
					}

					layer_.push_back(vector<vector<double>> { {max(0, s)}}); // ReLu

					sums += max(0, s); // relu(x*w)+relu(x*w)+...
				}

				// Расчёт изменения весов полносвязной сети (5 слой)
				for (int weightJ = 0; weightJ < weights.size(); weightJ++) {

					// sum(a,w)
					double sum = layer_[weightJ][0][0];

					double der = getLossDerivative2D(layer_, weights, weightJ, sum, sums, stoi(image.getName()));
					// ders_E6.push_back(der);

					for (int weightI = 0; weightI < layer4.size(); weightI++) {

						double step = -LEARNING_SPEED * der * layer4[weightI];

						double core_ = cores[layer_num - 1][weightJ][0][0][weightI];

						// cout << " (" << weightJ << "):" << step << endl;

						cores[layer_num - 1][weightJ][0][0][weightI] += step;
					}
				}
				
				/*
				weights.clear();
				for (int i = 0; i < cores_set.size(); i++)
					weights.push_back(cores_set[i][0][0]);

				for (int weightJ = 0; weightJ < weights.size(); weightJ++) {

					double sum = layer_[weightJ][0][0];

					double der = getLossDerivative2D(layer_, weights, weightJ, sum, sums, stoi(image.getName()));
					ders_E6.push_back(der);

				}
				*/

				// Производные выхода flatten (4 слой)
				vector<double> E4_x = ders_E4(weights, ders_E6);

				// Обратный flatten (итог 3 слоя)
				vector<vector<vector<double>>> E3_x = reverse_flatten(E4_x, 16, 4, 4);

				// Производная max_pooling_1 (3 слой)
				vector<vector<vector<double>>> E2_x;
				for (int k = 0; k < E3_x.size(); k++) {
					E2_x.push_back(reverse_max_pooling(E3_x[k], max_poses[1][k]));
				}

				// Производная max_pooling_1 (2 слой)
				vector<vector<vector<double>>> E1_x;
				for (int k = 0; k < E3_x.size(); k++) {
					E1_x.push_back(reverse_max_pooling(E2_x[k], max_poses[0][k]));
				}

				// Нахождение производных для ядер
				vector<vector<vector<double>>> ders_E1 = ders_cores(image.getImage(), E1_x);

				layer_num -= 1;
				for (int k = 0; k < ders_E1.size(); k++) {

					for (int y = 0; y < ders_E1[0].size(); y++) {
						for (int x = 0; x < ders_E1[0][0].size(); x++) {

							// Корректируем ядра
							cores[layer_num - 1][k][0][y][x] -= 50 * ders_E1[k][y][x];


						}

					}

				}

				/*
				// 1 слой
				layer_num -= 1;

				vector<vector<vector<double>>> cores_;

				cores_set = cores[layer_num - 1];


				for (int i = 0; i < cores_set.size(); i++)
					cores_.push_back(cores_set[i][0]);

				// Подсчитаем все производные
				for (int resultNum = 0; resultNum < 10; resultNum++) {

					for (int weightNum = 0; weightNum < cores_.size(); weightNum++) {

						for (int weightJ = 0; weightJ < cores_[0].size(); weightJ++) {

							for (int weightI = 0; weightI < cores_[0][weightJ].size(); weightI++) {

								double der = getLossDerivative3D(image.getImage(), cores_[weightNum], weightJ, weightI);

								cout << cores[layer_num - 1][weightNum][0][weightJ][weightI] << " + " << der << endl << endl << "---------------------------------------" << endl;

								cores[layer_num - 1][weightNum][0][weightJ][weightI] += (-LEARNING_SPEED * der);
							}
						}
					}
				}
				*/

			}
			
		}
	}


	if (!needToGenerate)
	{
		finCores.close();
		finBiases.close();
	}

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
	int layerNum = 1;

	int DEPTH = 1;
	int output_dim = 16;
	for (int i = 0; i < output_dim; i++)
	{

		for (int j = 0; j < DEPTH; j++)
		{
			writeMatrixInFile(foutCores, cores[layerNum-1][i][j]);
		}

		writeMatrixInFile(foutBiases, biases[layerNum - 1][i]);
	}




	// 5 слой
	layerNum += 1;

	DEPTH = 1;
	output_dim = 10;
	for (int i = 0; i < output_dim; i++)
	{
		for (int j = 0; j < DEPTH; j++)
		{
			writeMatrixInFile(foutCores, cores[layerNum - 1][i][j]);
		}
	}


	foutCores.close();
	foutBiases.close();

	return 0;
}