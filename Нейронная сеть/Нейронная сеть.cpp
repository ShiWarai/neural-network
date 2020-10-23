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

// a x b = c
vector<vector<double>> dot(vector<vector<double>> a, vector<vector<double>> b) {
    vector<vector<double>> c;

    for (int y1 = 0; y1 < a.size(); y1++) {
        c.push_back(vector<double> {});
        for (int x1 = 0; x1 < b[0].size(); x1++) {
            c[y1].push_back(0);
        }
    }
    
    if (a[0].size() != b.size())
        return c;

    for (int y1 = 0; y1 < a.size(); y1++) {
        for (int x1 = 0; x1 < b[0].size(); x1++) {
            for (int x = 0; x < a[0].size(); x++) {
                c[y1][x1] += a[y1][x] * b[x][x1];
            }
        }
    }

    return c;
}


void reluFunction(double **a, int x, int y) {
    for (int x1 = 0; x1 < x; x1++) {
        for (int y1 = 0; y1 < y; y1++) {
            a[x1][y1] = max(a[x1][y1], 0);
        }
    }

    return;
}

void directDistributionFunc(double **pic, vector<vector<float>> w, int size) {

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            pic[x][y] = pic[x][y] * w[x][y];
        }
    }
}

void convolutionFunc(double** pic, int currentSize) {
    vector<vector<double>> finalBuffer;

    for (int x = 0; x < currentSize; x += 2) {
        vector<double> buffer;
        for (int y = 0; y < currentSize; y += 2) {

            double c = max(pic[x][y], pic[x + 1][y]);
            c = max(c, pic[x][y + 1]);
            c = max(c, pic[x + 1][y + 1]);

            buffer.push_back(c);

            pic[x][y] = 0;
            pic[x + 1][y] = 0;
            pic[x][y + 1] = 0;
            pic[x + 1][y + 1] = 0;
        }
        finalBuffer.push_back(buffer);
    }

    for (int x = 0; x < currentSize/2; x ++) {
        for (int y = 0; y < currentSize/2; y ++) {
            pic[x][y] = finalBuffer[x][y];
            cout << setw(6) << setprecision(4) << pic[x][y] << " ";
        }
        cout << endl;
    }

    return;
}

void softmax(double** a, int x) {
    double sum = 0;

    for (int i = 0; i < x; i++)
        sum += a[i][0];

    for (int i = 0; i < x; i++)
        a[i][0] /= sum;

    return;
}

vector<vector<vector<float>>> generationWeights(vector<vector<vector<float>>> weights, unsigned PICTURE_SIZE) {
    int currentLayerSize;

    // Добавить функцию
    for (int i = 0; i < log2(PICTURE_SIZE); i++) {
        currentLayerSize = PICTURE_SIZE / pow(2, i);

        vector<vector<float>> layer;
        for (int k = 0; k < currentLayerSize; k++)
            layer.push_back(vector<float> {});

        for (int x = 0; x < currentLayerSize; x++) {
            for (int y = 0; y < currentLayerSize; y++) {
                layer[x].push_back((float) rand() / RAND_MAX);
            }
        }

        weights.push_back(layer);
    }

    return weights;
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

    cout << endl;
    cout << endl;


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



    // Создание весов
    vector<vector<vector<float>>> weights;
    weights = generationWeights(weights, PICTURE_SIZE);

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