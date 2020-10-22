#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <time.h>


using namespace std;


static unsigned short read_u16(FILE* fp)
{
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
}


static unsigned int read_u32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


static int read_s32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

void getPicture(int **picture, string pictureName) {

    FILE* pFile = fopen(pictureName.c_str(), "rb");

    // считываем заголовок файла
    BITMAPFILEHEADER header;

    header.bfType = read_u16(pFile);
    header.bfSize = read_u32(pFile);
    header.bfReserved1 = read_u16(pFile);
    header.bfReserved2 = read_u16(pFile);
    header.bfOffBits = read_u32(pFile);

    // считываем заголовок изображения
    BITMAPINFOHEADER bmiHeader;

    bmiHeader.biSize = read_u32(pFile);
    bmiHeader.biWidth = read_s32(pFile);
    bmiHeader.biHeight = read_s32(pFile);
    bmiHeader.biPlanes = read_u16(pFile);
    bmiHeader.biBitCount = read_u16(pFile);
    bmiHeader.biCompression = read_u32(pFile);
    bmiHeader.biSizeImage = read_u32(pFile);
    bmiHeader.biXPelsPerMeter = read_s32(pFile);
    bmiHeader.biYPelsPerMeter = read_s32(pFile);
    bmiHeader.biClrUsed = read_u32(pFile);
    bmiHeader.biClrImportant = read_u32(pFile);


    RGBQUAD** rgb = new RGBQUAD * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        rgb[i] = new RGBQUAD[bmiHeader.biHeight];
    }

    for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++) {
            rgb[i][j].rgbBlue = getc(pFile);
            rgb[i][j].rgbGreen = getc(pFile);
            rgb[i][j].rgbRed = getc(pFile);
        }
    }

    // выводим результат
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++)
            picture[i][j] = (rgb[i][j].rgbRed + rgb[i][j].rgbGreen + rgb[i][j].rgbBlue) / 3;
    }
    fclose(pFile);

    return;
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

	// Вывод названий и цифр
	for(int i = 0; i < trainingFiles.size(); i++)
		cout << "File name:" << trainingFiles[i][0] << "\nDigit:" << trainingFiles[i][1] << endl;

    /*
    for (int k = 0; k < trainingFiles.size(); k++) {

        int** image = new int* [PICTURE_SIZE];

        for (int i = 0; i < PICTURE_SIZE; i++)
            image[i] = new int[PICTURE_SIZE]();

        getPicture(image, PATH_S + trainingFiles[k][0]);

        // Тестовый вывод
        for (int y = 0; y < PICTURE_SIZE; y++) {
            for (int x = 0; x < PICTURE_SIZE; x++)
                cout << setw(4) << image[x][y] << " ";
            cout << endl;
        }

        cout << endl;
    }

    */

    // Создание весов
    vector<float **> scales;
    int currentLayerSize;

    // Добавить функцию
    for (int i = 0; i < log2(PICTURE_SIZE); i++) {
        currentLayerSize = PICTURE_SIZE / pow(2, i);

        float** layer = new float* [currentLayerSize];
        for (int k = 0; k < currentLayerSize; k++)
            layer[k] = new float[currentLayerSize]();

        for (int x = 0; x < currentLayerSize; x++) {
            for (int y = 0; y < currentLayerSize; y++) {
                layer[x][y] = floor( (float(rand()) / RAND_MAX)*100 ) / 100;
            }
        }

        scales.push_back(layer);
    }

    for (int k = 0; k < scales.size(); k++)
        cout << scales[k][0][0] << endl;

    const int epochs = 10;

    for (int epoch = 1; epoch <= epochs; epoch++) {

    }
	
	return 0;
}