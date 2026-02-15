#include <iostream>
#include <fstream>
#include <limits>
#include <cstdlib>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <ctime>
#include <filesystem>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
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

// Структура для накопления градиентов (2 обучаемых слоя)
struct Gradients {
	vector<vector<vector<vector<double>>>> layer1;  // 16 x 1 x 3 x 3
	vector<vector<vector<vector<double>>>> layer2;  // 10 x 1 x 1 x layer4_size
};

void initGradients(Gradients& g, const vector<vector<vector<vector<vector<double>>>>>& cores) {
	g.layer1 = cores[0];
	g.layer2 = cores[1];
	for (size_t i = 0; i < g.layer1.size(); i++)
		for (size_t j = 0; j < g.layer1[i].size(); j++)
			for (size_t y = 0; y < g.layer1[i][j].size(); y++)
				for (size_t x = 0; x < g.layer1[i][j][y].size(); x++)
					g.layer1[i][j][y][x] = 0;
	for (size_t i = 0; i < g.layer2.size(); i++)
		for (size_t j = 0; j < g.layer2[i].size(); j++)
			for (size_t y = 0; y < g.layer2[i][j].size(); y++)
				for (size_t x = 0; x < g.layer2[i][j][y].size(); x++)
					g.layer2[i][j][y][x] = 0;
}

void addGradient(Gradients& acc, const Gradients& g) {
	for (size_t i = 0; i < acc.layer1.size(); i++)
		for (size_t j = 0; j < acc.layer1[i].size(); j++)
			for (size_t y = 0; y < acc.layer1[i][j].size(); y++)
				for (size_t x = 0; x < acc.layer1[i][j][y].size(); x++)
					acc.layer1[i][j][y][x] += g.layer1[i][j][y][x];
	for (size_t i = 0; i < acc.layer2.size(); i++)
		for (size_t j = 0; j < acc.layer2[i].size(); j++)
			for (size_t y = 0; y < acc.layer2[i][j].size(); y++)
				for (size_t x = 0; x < acc.layer2[i][j][y].size(); x++)
					acc.layer2[i][j][y][x] += g.layer2[i][j][y][x];
}

void applyGradients(vector<vector<vector<vector<vector<double>>>>>& cores,
	const Gradients& grads, double lr, int batchSize) {
	if (batchSize < 1) return;
	double scale = lr / batchSize;
	for (size_t i = 0; i < cores[0].size(); i++)
		for (size_t j = 0; j < cores[0][i].size(); j++)
			for (size_t y = 0; y < cores[0][i][j].size(); y++)
				for (size_t x = 0; x < cores[0][i][j][y].size(); x++)
					cores[0][i][j][y][x] -= scale * grads.layer1[i][j][y][x];
	for (size_t i = 0; i < cores[1].size(); i++)
		for (size_t j = 0; j < cores[1][i].size(); j++)
			for (size_t y = 0; y < cores[1][i][j].size(); y++)
				for (size_t x = 0; x < cores[1][i][j][y].size(); x++)
					cores[1][i][j][y][x] -= scale * grads.layer2[i][j][y][x];
}

// Программа
// Использование: neural_network [путь_к_данным] [путь_к_cores] [путь_к_biases] [потоки]
//   путь_к_данным — папка с BMP (по умолчанию: data/)
//   путь_к_cores  — файл весов (по умолчанию: cores.dat)
//   путь_к_biases — файл смещений (по умолчанию: biases.dat)
//   потоки        — число потоков (по умолчанию: 1)
int main(int argc, char* argv[])
{
	const string DATA_PATH = (argc >= 2) ? string(argv[1]) : "data/";
	const string pathCores = (argc >= 3) ? string(argv[2]) : "cores.dat";
	const string pathBiases = (argc >= 4) ? string(argv[3]) : "biases.dat";
	int numThreads = (argc >= 5) ? atoi(argv[4]) : 1;
	if (numThreads < 1) numThreads = 1;

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
	const int BATCH_SIZE = 32;
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

	// Убрать остаток строки после cin >>, чтобы getline работал корректно
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
		{ string line; getline(cin, line); if (!line.empty()) { int v = atoi(line.c_str()); if (v >= 1) EPOCHS = v; } }
		cout << "Процент датасета для обучения? (1-100, по умолчанию 100)\n";
		{ string line; getline(cin, line); if (!line.empty()) { int v = atoi(line.c_str()); if (v >= 1 && v <= 100) datasetPercent = v; } }
	}

	int filesCount = (int)(trainingFiles.size() * datasetPercent / 100);
	if (filesCount < 1) filesCount = 1;

	vector<double> delta;
	double prediction;

	// Потокобезопасные счётчики и логирование
	atomic<int> win_atomic{0};
	atomic<int> all_atomic{0};
	atomic<int> processed_atomic{0};
	double loss_sum = 0;
	mutex stats_mutex;
	mutex log_mutex;

	int totalSamples = EPOCHS * filesCount;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= EPOCHS; epoch++) {

		if (straightOnly && epoch > 1)
			exit(0);

		double epochLossSum = 0;
		int epochSamples = 0;

		// Лямбда обработки одного файла (для инференса - straightOnly)
		auto processFile = [&](int fileNum) {
			if (fileNum >= (int)trainingFiles.size() || fileNum >= filesCount) return;

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);

			int output_dim = 16;
			vector<vector<vector<vector<double>>>> cores_set;
			vector<vector<vector<vector<double>>>> max_poses = { vector<vector<vector<double>>> {}, vector<vector<vector<double>>> {} };
			unsigned layer_num = 1;

			if (epoch == 1 && fileNum == 0) {
				int DEPTH = 1;
				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
						cores_set.push_back(generationCore(DEPTH, CORE_SIZE));
					else {
						cores_set.push_back(vector<vector<vector<double>>> {});
						for (int j = 0; j < DEPTH; j++) {
							cores_set[i].push_back(vector<vector<double>> {});
							cores_set[i][j] = readMatrixFromFile(finCores);
						}
					}
				}
				cores.push_back(cores_set);
			} else {
				cores_set = cores[layer_num - 1];
			}

			vector<vector<vector<double>>> layer1 = Dense(vector<vector<vector<double>>> {image.getImage()}, cores_set, output_dim, { {{}} });
			vector<vector<vector<double>>> layer3;
			for (int i = 0; i < layer1.size(); i++) {
				vector<vector<vector<double>>> buffer = max_pooling(layer1[i]);
				layer3.push_back(buffer[0]);
				max_poses[0].push_back(buffer[1]);
			}

			vector<double> layer4;
			for (int i = 0; i < layer3.size(); i++) {
				vector<double> new_matrix = flatten(layer3[i]);
				for (size_t k = 0; k < new_matrix.size(); k++) layer4.push_back(new_matrix[k]);
			}

			layer_num += 1;
			output_dim = 10;
			cores_set.clear();
			if (epoch == 1 && fileNum == 0) {
				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
						cores_set.push_back(vector<vector<vector<double>>> { { generationWeights(layer4.size()) } });
					else {
						cores_set.push_back(vector<vector<vector<double>>> {});
						for (int j = 0; j < 1; j++) {
							cores_set[i].push_back(vector<vector<double>> {});
							cores_set[i][j] = readMatrixFromFile(finCores);
						}
					}
				}
				cores.push_back(cores_set);
			} else {
				cores_set = cores[layer_num - 1];
			}

			vector<vector<vector<double>>> layer5;
			for (int i = 0; i < output_dim; i++) {
				double sum = 0;
				for (size_t k = 0; k < layer4.size(); k++) sum += layer4[k] * cores_set[i][0][0][k];
				layer5.push_back(vector<vector<double>> { {std::max(0.0, sum)}});
			}

			auto result = softmax(layer5);
			int prediction = 0;
			double maxProc = result[0];
			for (size_t i = 0; i < result.size(); i++) {
				if (result[i] > maxProc) { maxProc = result[i]; prediction = (int)i; }
			}

			double loss = getLoss(result, getUnitaryCode(result.size(), stoi(image.getName())));
			delta = getDelta(result, stoi(image.getName()));

			int correct = (stoi(image.getName()) == prediction) ? 1 : 0;
			win_atomic += correct;
			all_atomic += 1;
			epochLossSum += loss;
			epochSamples++;
			{ lock_guard<mutex> lock(stats_mutex); loss_sum += loss; }
			processed_atomic = (epoch - 1) * filesCount + (fileNum + 1);

			int w = win_atomic.load(), a = all_atomic.load(), p = processed_atomic.load();
			double avgLoss = (a > 0) ? (loss_sum / a) : 0;
			double accuracy = (a > 0) ? (100.0 * w / a) : 0;
			int progressTotal = straightOnly ? filesCount : totalSamples;
			int progressPct = (progressTotal > 0) ? (int)(100.0 * p / progressTotal) : 0;

			{ lock_guard<mutex> lock(log_mutex);
				cout << left << setw(16) << ("(" + trainingFiles[fileNum][0] + ")")
				     << " Prediction: " << prediction << "  |  Progress: " << p << "/" << progressTotal
				     << " (" << progressPct << "%)"
				     << "  |  Loss: " << fixed << setprecision(6) << avgLoss
				     << "  |  Accuracy: " << setprecision(2) << accuracy << "%" << endl;
			}
		};

		// Лямбда: вычислить градиенты для одного файла (для обучения с батчами)
		auto computeGradientsForFile = [&](int fileNum, Gradients& grad_out) -> tuple<double, int, int> {
			if (fileNum >= (int)trainingFiles.size() || fileNum >= filesCount) return {0, 0, 0};

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);

			int output_dim = 16;
			vector<vector<vector<vector<double>>>> cores_set;
			vector<vector<vector<vector<double>>>> max_poses = { vector<vector<vector<double>>> {}, vector<vector<vector<double>>> {} };
			unsigned layer_num = 1;

			if (epoch == 1 && fileNum == 0) {
				int DEPTH = 1;
				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
						cores_set.push_back(generationCore(DEPTH, CORE_SIZE));
					else {
						cores_set.push_back(vector<vector<vector<double>>> {});
						for (int j = 0; j < DEPTH; j++) {
							cores_set[i].push_back(vector<vector<double>> {});
							cores_set[i][j] = readMatrixFromFile(finCores);
						}
					}
				}
				cores.push_back(cores_set);
			} else {
				cores_set = cores[layer_num - 1];
			}

			vector<vector<vector<double>>> layer1 = Dense(vector<vector<vector<double>>> {image.getImage()}, cores_set, output_dim, { {{}} });
			vector<vector<vector<double>>> layer3;
			for (int i = 0; i < layer1.size(); i++) {
				vector<vector<vector<double>>> buffer = max_pooling(layer1[i]);
				layer3.push_back(buffer[0]);
				max_poses[0].push_back(buffer[1]);
			}

			vector<double> layer4;
			for (int i = 0; i < layer3.size(); i++) {
				vector<double> new_matrix = flatten(layer3[i]);
				for (size_t k = 0; k < new_matrix.size(); k++) layer4.push_back(new_matrix[k]);
			}

			layer_num += 1;
			output_dim = 10;
			cores_set.clear();
			if (epoch == 1 && fileNum == 0) {
				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
						cores_set.push_back(vector<vector<vector<double>>> { { generationWeights(layer4.size()) } });
					else {
						cores_set.push_back(vector<vector<vector<double>>> {});
						for (int j = 0; j < 1; j++) {
							cores_set[i].push_back(vector<vector<double>> {});
							cores_set[i][j] = readMatrixFromFile(finCores);
						}
					}
				}
				cores.push_back(cores_set);
			} else {
				cores_set = cores[layer_num - 1];
			}

			vector<vector<vector<double>>> layer5;
			for (int i = 0; i < output_dim; i++) {
				double sum = 0;
				for (size_t k = 0; k < layer4.size(); k++) sum += layer4[k] * cores_set[i][0][0][k];
				layer5.push_back(vector<vector<double>> { {std::max(0.0, sum)}});
			}

			auto result = softmax(layer5);
			int prediction = 0;
			double maxProc = result[0];
			for (size_t i = 0; i < result.size(); i++) {
				if (result[i] > maxProc) { maxProc = result[i]; prediction = (int)i; }
			}

			double loss = getLoss(result, getUnitaryCode(result.size(), stoi(image.getName())));
			int correct = (stoi(image.getName()) == prediction) ? 1 : 0;

			// Обратный ход — накопление градиентов
			if (grad_out.layer1.empty())
				initGradients(grad_out, cores);

			vector<vector<double>> weights;
			for (size_t i = 0; i < cores_set.size(); i++)
				weights.push_back(cores_set[i][0][0]);

			vector<vector<vector<double>>> layer_;
			vector<double> ders_E6;
			double sums = 0;

			for (size_t n = 0; n < weights.size(); n++) {
				double s = 0;
				for (size_t k = 0; k < weights[0].size(); k++) s += layer4[k] * weights[n][k];
				layer_.push_back(vector<vector<double>> { {std::max(0.0, s)}});
				sums += std::max(0.0, s);
			}

			for (size_t weightJ = 0; weightJ < weights.size(); weightJ++) {
				double sum = layer_[weightJ][0][0];
				double der = getLossDerivative2D(layer_, weights, (int)weightJ, sum, sums, stoi(image.getName()));
				ders_E6.push_back(der);

				for (size_t weightI = 0; weightI < layer4.size(); weightI++) {
					double grad = der * layer4[weightI];
					grad_out.layer2[weightJ][0][0][weightI] += grad;
				}
			}

			vector<double> E4_x = ders_E4(weights, ders_E6);
			vector<vector<vector<double>>> E3_x = reverse_flatten(E4_x, 16, 8, 8);

			vector<vector<vector<double>>> E1_x;
			for (size_t k = 0; k < E3_x.size(); k++)
				E1_x.push_back(reverse_max_pooling(E3_x[k], max_poses[0][k]));

			vector<vector<vector<double>>> ders_E1 = ders_cores(image.getImage(), E1_x, CORE_SIZE);
			layer_num -= 1;

			for (size_t k = 0; k < ders_E1.size(); k++) {
				for (size_t y = 0; y < ders_E1[0].size(); y++) {
					for (size_t x = 0; x < ders_E1[0][0].size(); x++) {
						grad_out.layer1[k][0][y][x] += ders_E1[k][y][x];
					}
				}
			}

			return {loss, correct, prediction};
		};

		if (straightOnly) {
			if (filesCount > 0) processFile(0);
			if (numThreads > 1 && filesCount > 1) {
				atomic<int> nextFile{1};
				vector<thread> threads;
				int numWorkers = min(numThreads, filesCount - 1);
				for (int t = 0; t < numWorkers; t++) {
					threads.emplace_back([&]() {
						while (true) {
							int f = nextFile++;
							if (f >= filesCount) break;
							processFile(f);
						}
					});
				}
				for (auto& th : threads) th.join();
			} else {
				for (int f = 1; f < filesCount; f++) processFile(f);
			}
		} else if (numThreads == 1) {
			// Обучение: последовательно (как раньше)
			for (int f = 0; f < filesCount; f++) {
				Gradients g;
				auto [loss, correct, prediction] = computeGradientsForFile(f, g);
				applyGradients(cores, g, LEARNING_SPEED, 1);

				win_atomic += correct;
				all_atomic += 1;
				{ lock_guard<mutex> lock(stats_mutex); loss_sum += loss; }
				processed_atomic = (epoch - 1) * filesCount + (f + 1);

				int w = win_atomic.load(), a = all_atomic.load(), p = processed_atomic.load();
				double avgLoss = (a > 0) ? (loss_sum / a) : 0;
				double accuracy = (a > 0) ? (100.0 * w / a) : 0;
				int progressPct = (totalSamples > 0) ? (int)(100.0 * p / totalSamples) : 0;

				{ lock_guard<mutex> lock(log_mutex);
					cout << left << setw(16) << ("(" + trainingFiles[f][0] + ")")
					     << " Prediction: " << prediction
					     << "  |  Epoch: " << epoch << "/" << EPOCHS
					     << "  |  Progress: " << p << "/" << totalSamples
					     << " (" << progressPct << "%)"
					     << "  |  Loss: " << fixed << setprecision(6) << avgLoss
					     << "  |  Accuracy: " << setprecision(2) << accuracy << "%" << endl;
				}
			}
		} else {
			// Обучение: батчи + многопоточность
			for (int batchStart = 0; batchStart < filesCount; batchStart += BATCH_SIZE) {
				int batchEnd = min(batchStart + BATCH_SIZE, filesCount);
				int actualBatchSize = batchEnd - batchStart;

				double batchLossSum = 0;
				int batchCorrect = 0;
				Gradients grad_accum;

				if (batchStart == 0 && epoch == 1) {
					Gradients g0;
					auto [loss, correct, _] = computeGradientsForFile(0, g0);
					batchLossSum += loss;
					batchCorrect += correct;
					initGradients(grad_accum, cores);
					addGradient(grad_accum, g0);
				} else {
					initGradients(grad_accum, cores);
				}

				vector<Gradients> threadGrads(numThreads);
				for (int t = 0; t < numThreads; t++)
					initGradients(threadGrads[t], cores);

				atomic<int> nextFile{batchStart == 0 && epoch == 1 ? 1 : batchStart};
				vector<thread> threads;
				int numWorkers = min(numThreads, filesCount);
				for (int t = 0; t < numWorkers; t++) {
					threads.emplace_back([&, t]() {
						while (true) {
							int f = nextFile++;
							if (f >= batchEnd) break;
							auto [loss, correct, _] = computeGradientsForFile(f, threadGrads[t]);
							lock_guard<mutex> lock(stats_mutex);
							batchLossSum += loss;
							batchCorrect += correct;
						}
					});
				}
				for (auto& th : threads) th.join();

				for (int t = 0; t < numThreads; t++)
					addGradient(grad_accum, threadGrads[t]);

				// Linear scaling rule: lr *= batch_size для эквивалентности online SGD
				applyGradients(cores, grad_accum, LEARNING_SPEED * actualBatchSize, actualBatchSize);

				win_atomic += batchCorrect;
				all_atomic += actualBatchSize;
				{ lock_guard<mutex> lock(stats_mutex); loss_sum += batchLossSum; }
				processed_atomic = (epoch - 1) * filesCount + batchEnd;

				int w = win_atomic.load(), a = all_atomic.load(), p = processed_atomic.load();
				double avgLoss = (a > 0) ? (loss_sum / a) : 0;
				double accuracy = (a > 0) ? (100.0 * w / a) : 0;
				int totalBatches = (filesCount + BATCH_SIZE - 1) / BATCH_SIZE;
				int batchIdx = batchStart / BATCH_SIZE + 1;

				{ lock_guard<mutex> lock(log_mutex);
					cout << "Batch " << batchIdx << "/" << totalBatches
					     << "  |  Epoch: " << epoch << "/" << EPOCHS
					     << "  |  Progress: " << p << "/" << totalSamples
					     << "  |  Loss: " << fixed << setprecision(6) << avgLoss
					     << "  |  Accuracy: " << setprecision(2) << accuracy << "%" << endl;
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