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

vector<vector<double>> matrixExpansion(vector<vector<double>> matrix, unsigned padding) {

    auto new_matrix = createFilledVector(matrix.size() + padding, matrix[0].size() + padding);

    // Заполнение расширения 
    for (int padd_ = 1; padd_ <= padding * 2; padd_++) {
        int panding = (int)ceil((double)padd_ / 2) - 1;


        // Для север-запад
        if ((padd_ - 1) % 4 == 0 || (padd_ - 2) % 4 == 0)
        {
            // Нечётное дополнение
            if (padd_ % 2 == 1)
            {
                for (int i = 0, j = 0; i < new_matrix.size() - padding; i++, j++)
                {
                    if (i - padding < 0 && padd_ != padding * 2)
                    {
                        new_matrix[padd_ - 1][i] = matrix[0][0];
                        j--;
                    }
                    else
                    {
                        new_matrix[padd_ - 1][i] = matrix[0][j];
                    }
                }
            }
            // Чётное дополнение
            else
            {
                for (int i = new_matrix.size() - 1, j = matrix.size() - 1; i >= 0 + padding; i--, j--)
                {
                    if (i + padding > new_matrix.size() - 1 && padd_ != padding * 2)
                    {
                        new_matrix[i][padd_ - 1] = matrix[matrix.size() - 1][0];
                        j++;
                    }
                    else
                    {
                        new_matrix[i][padd_ - 1] = matrix[j][0];
                    }
                }
            }
        }
        // Для юг-восток
        else
        {
            // Нечётное дополнение
            if (padd_ % 2 == 1)
            {
                for (int i = new_matrix.size() - 1, j = matrix.size() - 1; i >= 0 + padding; i--, j--)
                {
                    if (i + padding > new_matrix.size() - 1 && padd_ != padding * 2)
                    {
                        new_matrix[new_matrix.size() - padd_][i] = matrix[matrix.size() - 1][matrix.size() - 1];
                        j++;
                    }
                    else
                    {
                        new_matrix[new_matrix.size() - padd_][i] = matrix[0][j];
                    }
                }
            }
            // Чётное дополнение
            else
            {
                for (int i = 0, j = 0; i < new_matrix.size() - padding; i++, j++)
                {
                    if (i - padding < 0 && padd_ != padding * 2)
                    {
                        new_matrix[new_matrix.size() - padd_][i] = matrix[0][matrix.size() - 1];
                        j--;
                    }
                    else
                    {
                        new_matrix[new_matrix.size() - padd_][i] = matrix[0][j];
                    }
                }
            }
        }

    }

    // Заполнение центра

    return new_matrix;

}

// Главная функция прямого распространения
vector<vector<double>> directDistributionFunc(vector<vector<vector<double>>> pic, vector<vector<vector<double>>> core) {
    unsigned y_size = pic[0].size();
    unsigned x_size = pic[0][0].size();


    auto processed_pic = createFilledVector(y_size, x_size);

    int dimensions = pic.size();

    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            double sum = 0;

            for (int dim = 0; dim < dimensions; dim++) {

                // Работа с плоскостью
                vector<vector<double>> flat = matrixExpansion(pic[dim], core.size() - 1);


            }

            processed_pic[y][x] = sum;
        }
    }

    return processed_pic;
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
vector<vector<vector<double>>> generationCore(unsigned CORE_SIZE, unsigned DEPTH) {
    vector<vector<vector<double>>> core;

    for (int i = 0; i < DEPTH; i++)
    {
        core.push_back(vector<vector<double>> {});

        core[i] = createFilledVector(CORE_SIZE, CORE_SIZE);

        for (int y1 = 0; y1 < CORE_SIZE; y1++) {
            for (int x1 = 0; x1 < CORE_SIZE; x1++) {
                core[i][y1][x1] = (double)(rand() % 10);
            }
        }
    }

    return core;
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
        for (int fileNum = 0; fileNum < trainingFiles.size(); fileNum++) {

            BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);
            cout << "(" << epoch << ", " << trainingFiles[fileNum][0] << ") :" << endl;
            
            // Слой 1
            vector<vector<vector<vector<double>>>> cores_set;
            int output_dim = 32;

            if (epoch == 1 && fileNum == 0) {

                for (int i = 0; i < output_dim; i++) {
                    cores_set.push_back(vector<vector<vector<double>>> {});
                    cores_set[i] = generationCore(2, 1);
                }

                cores[0] = cores_set;
            }
            else {
                cores_set = cores[0];
            }

            vector<vector<vector<double>>> processed_images;
            for (int i = 0; i < output_dim; i++) {
                auto core = cores_set[i];

                processed_images.push_back(directDistributionFunc(vector<vector<vector<double>>> {image.getImage()}, core));
            }







            
            delta.clear();
        }



    }
	
	return 0;
}