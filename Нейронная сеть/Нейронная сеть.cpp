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

double max4(double a, double b, double c, double d)
{
    a = max(a, b);
    c = max(a, c);
    d = max(c, d);
    return d;
}

// Создание нулевой матрицы Y x X
vector<vector<double>> createFilledVector(int y, int x) {
    vector<vector<double>> c;

    for (int y1 = 0; y1 < y; y1++) {
        c.push_back(vector<double> {});
        for (int x1 = 0; x1 < x; x1++) {
            c[y1].push_back(0);
        }
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

// Поэлементный максимум
vector<vector<double>> reluFunction(vector<vector<double>> a) {

    for (int y1 = 0; y1 < a.size(); y1++) {
        for (int x1 = 0; x1 < a[0].size(); x1++) {
            a[y1][x1] = max(a[y1][x1], 0);
        }
    }

    return a;
}

// Прямое распространение
void directDistributionFunc(double **pic, vector<vector<float>> w, int size) {

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            pic[x][y] = pic[x][y] * w[x][y];
        }
    }
}

// Свёртка
vector<vector<double>> convolutionFunc(vector<vector<double>> a) {
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

    for (int y = 0; y < currentSize/2; y++) {
        for (int x = 0; x < currentSize/2; x++) {
            a[y][x] = finalBuffer[y][x];
            cout << setw(6) << setprecision(4) << a[y][x] << " ";
        }
        cout << endl;
    }

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

// Первое заполнение ядер
vector<vector<vector<double>>> generationCores(unsigned CORE_SIZE, unsigned CORES_NUM) {
    vector<vector<vector<double>>> cores;

    for(int i = 0; i < CORES_NUM; i++)
    {
        cores.push_back(vector<vector<double>> {});

        cores[i] = createFilledVector(CORE_SIZE,CORE_SIZE);

        for (int y1 = 0; y1 < CORES_NUM; y1++) {
            for (int x1 = 0; x1 < CORES_NUM; x1++) {
                cores[i][y1][x1] = (double)(rand() % 100) / 10;
            }
        }
    }

    return cores;
}

int main()
{
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
	for(int i = 0; i < trainingFiles.size(); i++)
		cout << "File name:" << trainingFiles[i][0] << "\nDigit:" << trainingFiles[i][1] << endl;

    for (int k = 0; k < trainingFiles.size(); k++) {

        BMP_BW image(trainingFiles[k][1], (string)(PATH_S + trainingFiles[k][0]), false);

        // Тестовый вывод
        for (int y = PICTURE_SIZE - 1; y >= 0; y--) {
            for (int x = 0; x < PICTURE_SIZE; x++)
                cout << setw(3) << setprecision(3)<< image[y][x] << " ";
            cout << endl;
        }

        cout << endl;
    }

    // Создание ядер
    vector<vector<vector<double>>> cores;
    cores = generationCores(2, 32);

    for (int i = 0; i < 32; i++)
        cout << cores[i][0][0]<<endl;

    const int epochs = 1;

    vector<double> delta;
    double prediction;

    // Итерации обучения (прямой и обратный ход)
    for (int epoch = 1; epoch <= epochs; epoch++) {
        for (int fileNum = 0; fileNum < trainingFiles.size(); fileNum++) {
            cout << "(" << epoch << ", " << trainingFiles[fileNum][1] << ") :" << endl;
            
            
            delta.clear();
        }
    }
	
	return 0;
}