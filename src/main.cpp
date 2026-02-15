#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <ctime>
#include <filesystem>
#include "BMP_reading.h"

namespace fs = std::filesystem;

using namespace std;
using namespace BMP;

#include "functions.h"
#include "matrixFunctions.h"
#include "neuralNetwork.h"
#include "neural_IO.h"
#include "testingArea.h"

//vector<vector<double>> generationBias(int a, int b, double koef = 10);
vector<vector<vector<double>>> Dense(vector<vector<vector<double>>> input, vector<vector<vector<vector<double>>>> cores_set, unsigned outputLayers, vector<vector<vector<double>>>  biases_set = { {{}} });

// Программа
// Использование: neural_network [путь_к_данным] [путь_к_cores] [путь_к_biases]
//   путь_к_данным — папка с BMP (по умолчанию: data/)
//   путь_к_cores  — файл весов (по умолчанию: cores.dat)
//   путь_к_biases — файл смещений (по умолчанию: biases.dat)
int main(int argc, char* argv[])
{
	const string DATA_PATH = (argc >= 2) ? string(argv[1]) : "data/";
	const string pathCores = (argc >= 3) ? string(argv[2]) : "cores.dat";
	const string pathBiases = (argc >= 4) ? string(argv[3]) : "biases.dat";

	string pathData = DATA_PATH;
	if (!pathData.empty() && pathData.back() != '/' && pathData.back() != '\\')
		pathData += '/';

	srand(abs(rand() - time(NULL)) * 100);
	setlocale(LC_ALL, "ru");
	// cout.setf(ios::fixed);

	ifstream finCores/*, finBiases*/;
	ofstream foutCores/*, foutBiases*/;
	
	static vector<vector<string>> trainingFiles;

	const unsigned int PICTURE_SIZE = 16;
	const unsigned int CORE_SIZE = 3;
	const double LEARNING_SPEED = 1000;
	const string PATH_S = pathData;
	// Чтение файлов
	{
		if (fs::exists(pathData) && fs::is_directory(pathData))
		{
			for (const auto& entry : fs::directory_iterator(pathData))
			{
				if (!entry.is_regular_file()) continue;
				string fileName = entry.path().filename().string();
				if (fileName == "." || fileName == "..") continue;
				string digit = fileName.substr(0, fileName.find(char(32)));
				trainingFiles.push_back(vector<string>{fileName, digit});
			}
		}

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
	//vector<vector<vector<vector<double>>>> biases;


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
		//finBiases.open(pathBiases);
		if (!finCores.is_open()/* || !finBiases.is_open()*/)
		{
			cout << "Ошибка открытия файла для чтения";
			return -5;
		}
	}

	int EPOCHS = 3;
	int datasetPercent = 100;
	if (!straightOnly) {
		cout << "Число эпох обучения? (по умолчанию 3)\n";
		cin >> EPOCHS;
		if (EPOCHS < 1) EPOCHS = 3;
		cout << "Процент датасета для обучения? (1-100, по умолчанию 100)\n";
		cin >> datasetPercent;
		if (datasetPercent < 1) datasetPercent = 1;
		if (datasetPercent > 100) datasetPercent = 100;
	}

	int filesCount = (int)(trainingFiles.size() * datasetPercent / 100);
	if (filesCount < 1) filesCount = 1;

	vector<double> delta;
	double prediction;

	int win = 0;
	int all = 0;
	int totalSamples = EPOCHS * filesCount;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= EPOCHS; epoch++) {

		if (straightOnly && epoch > 1)
			exit(0);

		double epochLossSum = 0;
		int epochSamples = 0;

		for (int fileNum = 0; (fileNum < trainingFiles.size()) && (fileNum < filesCount); fileNum++) { // trainingFiles.size()

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);

			// Прямой ход
			
			int output_dim = 16;
			vector<vector<vector<vector<double>>>> cores_set;
			//vector<vector<vector<double>>> biases_set;

			vector<vector<vector<vector<double>>>> max_poses = 
				{ vector<vector<vector<double>>> {}, vector<vector<vector<double>>> {} };

			// Слой 1

			unsigned layer_num = 1;

			// Генерация или чтение набора ядер и смещения
			if (epoch == 1 && fileNum == 0)
			{
				int DEPTH = 1;

				for (int i = 0; i < output_dim; i++)
				{
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});
						cores_set[i] = generationCore(DEPTH, CORE_SIZE);

						//biases_set.push_back(generationBias(image.getHeight(), image.getWidth()));
					}
					else
					{						
						cores_set.push_back(vector<vector<vector<double>>> {});

						for (int j = 0; j < DEPTH; j++)
						{
							cores_set[i].push_back(vector<vector<double>> {});

							cores_set[i][j] = readMatrixFromFile(finCores);
						}

						//biases_set.push_back(readMatrixFromFile(finBiases));
					}
					
				}

				cores.push_back(cores_set);
				//biases.push_back(biases_set);
			}
			else
			{
				cores_set = cores[layer_num - 1];
				//biases_set = biases[layer_num - 1];
			}

			vector<vector<vector<double>>> layer1 = Dense(vector<vector<vector<double>>> {image.getImage()}, cores_set, output_dim, { {{}} });

			/*
			// 2 слой
			vector<vector<vector<double>>> layer2;

			for (int i = 0; i < layer1.size(); i++) {
				vector<vector<vector<double>>> buffer = max_pooling(layer1[i]);

				layer2.push_back(buffer[0]);
				max_poses[0].push_back(buffer[1]);
			}
			*/


			// 3 слой
			vector<vector<vector<double>>> layer3;

			for (int i = 0; i < layer1.size(); i++) {
				vector<vector<vector<double>>> buffer = max_pooling(layer1[i]);

				layer3.push_back(buffer[0]);
				max_poses[0].push_back(buffer[1]);
			}


			// 4 слой
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
			layer_num += 1;

			output_dim = 10;
			cores_set.clear();
			//biases_set.clear();

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

				layer5.push_back(vector<vector<double>> { {std::max(0.0, sum)}}); // ReLu
			}

			// Просчёт вероятностей
			auto result = softmax(layer5);

			// Итоговое предсказание
			int prediction = 0;

			double maxProc = result[0];
			for (int i = 0; i < result.size(); i++) {
				if (result[i] > maxProc) {
					maxProc = result[i];
					prediction = i;
				}
			}

			double loss = getLoss(result, getUnitaryCode(result.size(), stoi(image.getName())));
			delta = getDelta(result, stoi(image.getName()));

			win += (1 ? stoi(image.getName()) == prediction : 0);
			all += 1;

			epochLossSum += loss;
			epochSamples++;

			double avgLoss = epochLossSum / epochSamples;
			double accuracy = ((double)win / (double)all) * 100.0;
			int processed = (epoch - 1) * filesCount + (fileNum + 1);
			int progressTotal = straightOnly ? filesCount : totalSamples;
			int progressPct = straightOnly
				? (int)(100.0 * processed / filesCount)
				: (int)(100.0 * processed / totalSamples);

			// Единый формат вывода: файл, предсказание, эпоха (при обучении), прогресс, loss, accuracy
			cout << left << setw(16) << ("(" + trainingFiles[fileNum][0] + ")")
			     << " Prediction: " << prediction;
			if (!straightOnly) {
				cout << "  |  Epoch: " << epoch << "/" << EPOCHS;
			}
			cout << "  |  Progress: " << processed << "/" << progressTotal
			     << " (" << progressPct << "%)"
			     << "  |  Loss: " << fixed << setprecision(6) << avgLoss
			     << "  |  Accuracy: " << setprecision(2) << accuracy << "%" << endl;

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

					layer_.push_back(vector<vector<double>> { {std::max(0.0, s)}}); // ReLu

					sums += std::max(0.0, s); // relu(x*w)+relu(x*w)+...
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
				vector<vector<vector<double>>> E3_x = reverse_flatten(E4_x, 16, 8, 8);

				// Производная max_pooling_1 (3 слой)
				vector<vector<vector<double>>> E1_x;
				for (int k = 0; k < E3_x.size(); k++) {
					E1_x.push_back(reverse_max_pooling(E3_x[k], max_poses[0][k]));
				}


				// Нахождение производных для ядер
				vector<vector<vector<double>>> ders_E1 = ders_cores(image.getImage(), E1_x, CORE_SIZE);

				layer_num -= 1;
				for (int k = 0; k < ders_E1.size(); k++) {

					for (int y = 0; y < ders_E1[0].size(); y++) {
						for (int x = 0; x < ders_E1[0][0].size(); x++) {

							// Корректируем ядра
							cores[layer_num - 1][k][0][y][x] -= LEARNING_SPEED * ders_E1[k][y][x];


						}

					}

				}

			}
		}
	}


	if (!needToGenerate)
	{
		finCores.close();
		//finBiases.close();
	}

	foutCores.open(pathCores);
	//foutBiases.open(pathBiases);
	if (!foutCores.is_open()/* || !foutBiases.is_open()*/)
	{
		cout << "Ошибка открытия файла для записи";
		return -6;
	}
	foutCores.clear();
	//foutBiases.clear();

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

		//writeMatrixInFile(foutBiases, biases[layerNum - 1][i]);
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
	//foutBiases.close();

	return 0;
}