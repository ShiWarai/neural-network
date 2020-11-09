#include "include.h"
#include "functions.h"

// Запись матрицы в файл
void writeMatrixInFile(ofstream &fout, vector<vector<double>> matrix)
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
vector<vector<double>> readMatrixFromFile(ifstream &fin)
{
	int x, y;

	fin >> x;
	fin >> y;

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

// Максимум из 4 переменных
double max4(double a, double b, double c, double d)
{
	a = max(a, b);
	a = max(a, c);
	a = max(c, d);
	return a;
}

// Вывод матрицы в консоль
void consoleOutMatrix(vector<vector<double>> a) {
	for (int y = 0; y < a.size(); y++) {
		for (int x = 0; x < a[0].size(); x++)
			cout << setw(6) << setprecision(3) << a[y][x];
		cout << endl;
	}

	cout << endl;

	return;
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
vector<vector<double>> sumElements(vector<vector<double>> a, vector<vector<double>> b) {

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

// Сумма всех элементов
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

// Вычисление потерь
double getLoss(vector<double> y, vector<double> solution) {
	if (y.size() != solution.size())
		exit(0);

	double loss = 0;

	for (int i = 0; i < y.size(); i++)
		loss += pow(solution[i] - y[i], 2);

	return  loss / (double)y.size();
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
vector<vector<double>> getProcessedMatrix(vector<vector<vector<double>>> matrix, vector<vector<vector<double>>> core, vector<vector<double>> bias) {

	if (core.size() != matrix.size())
		exit(0);

	unsigned y_size = matrix[0].size();
	unsigned x_size = matrix[0][0].size();


	auto processed_pic = createFilledVector(y_size, x_size);

	int dimensions = matrix.size();
	int panding_ = (int)ceil((double)(core.size() - 1) / 2); // Определяем начало гл. массива

	vector<vector<double>> flat;
	for (int dim = 0; dim < dimensions; dim++) {

		// Работа с плоскостью
		flat = matrixExpansion(matrix[dim], core[0].size() - 1); // Получаем расширенную матрицу плоскости

		for (int y = 0; y < y_size; y++) {
			for (int x = 0; x < x_size; x++){

				auto slice = matrixSlicer(flat, y, x, core[dim].size(), core[dim][0].size()); // Получаем кусок матрицы
				slice = dot(slice, core[dim]); // Находим произведение

				processed_pic[y][x] += elementsSum(slice); // Складываем слои
			}
		}
	}

	processed_pic = sumElements(processed_pic, bias);

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
vector<double> softmax(vector < vector<vector<double>>> a) {
	double sum = 0;
	vector<double> c;

	for (int i = 0; i < a.size(); i++)
		sum += a[i][0][0];

	if (sum == 0)
		return vector<double>{NULL};

	for (int i = 0; i < a.size(); i++)
		a[i][0][0] /= sum;

	for (int i = 0; i < a.size(); i++)
		c.push_back(a[i][0][0]);

	return c;
}

// Развёртывание
vector<double> flatten(vector<vector<double>> a) {
	vector<double> b;
	for (int y = 0; y < a.size(); y++) {
		for (int x = 0; x < a.size(); x++)
			b.push_back(a[y][x]);
	}

	return b;
}

// Первое заполнение ядер
vector<vector<vector<double>>> generationCore(unsigned DEPTH, unsigned CORE_SIZE) {
	vector<vector<vector<double>>> core;

	for (int i = 0; i < DEPTH; i++)
	{
		core.push_back(vector<vector<double>> {});

		core[i] = createFilledVector(CORE_SIZE, CORE_SIZE);

		for (int y1 = 0; y1 < CORE_SIZE; y1++) {
			for (int x1 = 0; x1 < CORE_SIZE; x1++) {
				core[i][y1][x1] = 1 - ((double)rand() /(RAND_MAX)) * 2;
			}
		}
	}

	return core;
}

// Первое заполнение смещения
vector<vector<double>> generationBias(int a, int b, double koef) {
	vector<vector<double>> bias;

	bias = createFilledVector(a, b);

	for (int y1 = 0; y1 < a; y1++) {
		for (int x1 = 0; x1 < b; x1++) {
			bias[y1][x1] = -0.5 + ((double)rand() * koef / (RAND_MAX ));
		}
	}

	return bias;
}

vector<double> getUnitaryCode(int vectorSize, int num) {
	vector<double> a;

	for (int i = 0; i < vectorSize; i++)
		a.push_back(1 ? num == i : 0);

	return a;
}

// Получить весы в вектор-строке
vector<double> generationWeights(int a) {
	return generationBias(1, a, 2)[0];
}

// Состоит из свёртки и сжатия
vector<vector<vector<double>>> Dense(vector<vector<vector<double>>> input, vector<vector<vector<vector<double>>>> cores_set, vector<vector<vector<double>>>  biases_set, unsigned outputLayers, bool do_max_pooling) {

	vector<vector<vector<double>>> layer;

	vector<vector<double>> new_matrix;

	vector<vector<vector<double>>> core;
	vector<vector<double>> bias;
	for (int i = 0; i < outputLayers; i++) {
		core = cores_set[i];
		bias = biases_set[i];

		new_matrix = getProcessedMatrix(input, core, bias);

		// Max_pooling 2x2
		if(do_max_pooling)
			new_matrix = max_pooling(new_matrix);

		// Добавляем уже сжатый слой
		layer.push_back(new_matrix);
	}

	return layer;
}

// Получить разницу
vector<double> getDelta(vector<double> a, int solution) {
	for (int i = 0; i < a.size(); i++) {

		if (i == solution)
			a[i] = abs(double(1) - a[i]);
		else
			a[i] = abs(double(0) - a[i]);

	}

	return a;
}

// Получить производную от потери
double getLossDerivative(vector<double> a, vector<vector<double>> w, int j, int i, double solution) {

	if (j >= w.size() || i >= w[0].size())
		exit(0);

	// Вычислим производную по формуле d(loss(w)) / d(w) = 
	// = (-2*b_i) / k * (solution - x(w)) * x'(w), где
	// x(w) = softmax(g(w)), sums(a,w)), а x'(w) = g'(w) * (sums - g(w)) / sums^2

	// sum(a,w)
	double sum = 0;

	for (int k = 0; k < a.size(); k++)
		sum += (w[j][k] * a[k]);

	// x(w) = softmax(sum, sums(a,w))

	vector<vector<vector<double>>> layer;

	double sums = 0;
	for (int n = 0; n < w.size(); n++) {
		double s = 0;

		for (int k = 0; k < w[0].size(); k++) {
			s += a[k] * w[n][k];
		}

		layer.push_back(vector<vector<double>> { {max(0, s)}}); // ReLu
		sums += max(0, s);
	}

	if (softmax(layer) == vector<double> {NULL})
		return 0;

	double softmax_result = softmax(layer)[j];

	// g'(w) = b_i * { 0: sum < 0; 1: sum >= 0;

	double d_g = a[i] * (0 ? sum < 0 : 1);

	// g(w) = relu(sum)

	double g = max(sum,0);

	// x'(w) = g'(w) * (sums - g(w)) / sums^2

	double d_softmax = d_g * (sums - g) / pow(sums, 2);
	
	return ((-2 * a[i]) / w.size()) * (solution - softmax_result) * d_softmax;
}

void testNet() {
	// Инициализация ядер и смещения
	vector<vector<vector<vector<vector<double>>>>> cores;
	vector<vector<vector<vector<double>>>> biases;

	vector<vector<vector<double>>> data{ {{0,0,1,0,0,0,0,0},{0}},{{0,0,0,0,0,0,1,0},{1}}, {{0,0,0,0,0,0,0,1},{1}}, {{0,0,0,0,1,0,0,0},{1}}, {{1,0,0,0,0,0,0,0},{0}}, {{0,0,0,1,0,0,0,0},{0}},{{0,1,0,0,0,0,0,0},{0}},{{0,0,0,0,0,1,0,0},{1}}, {{0,0,0,0,0,1,0,0},{1}}, {{0,1,0,0,0,0,0,0},{0}},{{0,0,0,0,0,0,0,1},{1}},{{1,0,0,0,0,0,0,0},{0}},{{1,0,0,0,0,0,0,0},{0}},{{0,0,0,0,0,1,0,0},{1}},{{0,0,0,0,1,0,0,0},{1}},{{0,0,1,0,0,0,0,0},{0}} };

	const int epochs = 20;

	vector<double> delta;
	double prediction;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= epochs; epoch++) {

		for (int fileNum = 0; fileNum < data.size(); fileNum++) {

			// Прямой ход

			vector<double> layer4 = data[fileNum][0];

			int answer = data[fileNum][1][0];

			int layer_num = 1;
			int output_dim = 2;

			cout << "Ожидаемо: " << answer << endl;

			// Генерация или чтение набора ядер и смещения
			vector<vector<vector<vector<double>>>> cores_set;
			vector<vector<vector<double>>> biases_set;

			cores_set.clear();
			biases_set.clear();

			// Генерация или чтение набора ядер
			if (epoch == 1 && fileNum == 0) {

				for (int i = 0; i < output_dim; i++) {
					cores_set.push_back(vector<vector<vector<double>>> {});

					cores_set[i].push_back(vector<vector<double>> { generationWeights(layer4.size()) });
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

			cout << endl << "Result:" << endl;
			for (int x = 0; x < layer5.size(); x++)
				cout << layer5[x][0][0] << endl;
			cout << endl;

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

			cout << "Prediction:" << prediction << endl << endl;

			delta = getDelta(result, answer);

			cout << "Summary loss:" << getLoss(result, getUnitaryCode(result.size(), answer)) << endl << endl;

			for (int i = 0; i < delta.size(); i++)
				cout << setprecision(6) << setw(10) << delta[i];
			cout << endl;

			// Обратный ход

			vector<vector<double>> weights;

			for (int i = 0; i < cores_set.size(); i++)
				weights.push_back(cores_set[i][0][0]);

			for (int weightJ = 0; weightJ < weights.size(); weightJ++) {

				for (int weightI = 0; weightI < layer4.size(); weightI++) {
					double der = getLossDerivative(layer4, weights, weightJ, weightI, 1 ? weightJ == answer : 0);
					// cout << cores[0][weightJ][0][0][weightI] << " + " << (-0.5 * der) << endl << endl << "---------------------------------------" << endl;

					cores[0][weightJ][0][0][weightI] += (-1 * der);
				}
			}
		}
	}

	int success = 0;
	int failure = 0;

	// Вывод результатов:

	data = { {{0,0,0,0,0,0,1,0},{1}},{{0,1,0,0,0,0,0,0},{0}},{{0,0,0,0,0,1,0,0},{1}},{{1,0,0,0,0,0,0,0},{0}},{{0,0,0,0,0,0,0,1},{1}},{{0,0,1,0,0,0,0,0},{0}}, };

	cout << "Testing..." << endl << endl;

	for (int fileNum = 0; fileNum < data.size(); fileNum++) {

		// Прямой ход

		vector<double> layer4 = data[fileNum][0];

		int answer = data[fileNum][1][0];

		int layer_num = 1;
		int output_dim = 2;

		cout << "Ожидаемо: " << answer << endl;

		// Генерация или чтение набора ядер и смещения
		vector<vector<vector<vector<double>>>> cores_set;
		vector<vector<vector<double>>> biases_set;

		cores_set.clear();
		biases_set.clear();

		cores_set = cores[layer_num - 1];

		vector<vector<vector<double>>> layer5;

		for (int i = 0; i < output_dim; i++) {
			double sum = 0;

			for (int k = 0; k < layer4.size(); k++) {
				sum += layer4[k] * cores_set[i][0][0][k];
			}

			layer5.push_back(vector<vector<double>> { {max(0, sum)}}); // ReLu
		}

		cout << endl << "Result:" << endl;
		for (int x = 0; x < layer5.size(); x++)
			cout << layer5[x][0][0] << endl;
		cout << endl;

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

		cout << "Prediction:" << prediction << endl << endl;

		delta = getDelta(result, answer);

		cout << "Summary loss:" << getLoss(result, getUnitaryCode(result.size(), answer)) << endl << endl;

		if (answer == prediction)
			success += 1;
		else
			failure += 1;
	}

	cout << "Success: " << success << endl;
	cout << "Failure: " << failure << endl;

	return;
}

// Программа
int main()
{
	srand(abs(rand() - time(NULL)) * 100);
	setlocale(LC_ALL, "ru");
	// cout.setf(ios::fixed);

	ifstream finCores, finBiases;
	ofstream foutCores, foutBiases;
	string pathCores = "cores.txt", pathBiases = "biases.txt";


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

	const int epochs = 1;

	vector<double> delta;
	double prediction;

	// Итерации обучения (прямой и обратный ход)
	for (int epoch = 1; epoch <= epochs; epoch++) {

		for (int fileNum = 0; fileNum < trainingFiles.size(); fileNum++) {

			BMP_BW image(trainingFiles[fileNum][1], (string)(PATH_S + trainingFiles[fileNum][0]), false);
			cout << "(" << epoch << ", " << trainingFiles[fileNum][0] << ") :" << endl;

			// Прямой ход

			// Слой 1

			cout << "1 LAYER" << endl;
			
			int output_dim = 16;

			// Генерация или чтение набора ядер и смещения
			vector<vector<vector<vector<double>>>> cores_set;
			vector<vector<vector<double>>> biases_set;
			unsigned layer_num = 1;

			if (epoch == 1 && fileNum == 0)
			{

				for (int i = 0; i < output_dim; i++)
				{
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});
						int DEPTH = 1, CORE_SIZE = 2;
						cores_set[i] = generationCore(DEPTH, CORE_SIZE);

						biases_set.push_back(generationBias(image.getHeight(), image.getWidth()));
					}
					else
					{
						if (i == 0)
						{
							finCores.open(pathCores);
							finBiases.open(pathBiases);
							if (!finCores.is_open() || !finBiases.is_open())
							{
								cout << "Ошибка открытия файла для чтения";
								return -5;
							}
						}
						
						cores_set.push_back(vector<vector<vector<double>>> {});

						int DEPTH = 1;

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

				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});
						int DEPTH = layer1.size();
						cores_set[i] = generationCore(DEPTH, 2);

						biases_set.push_back(generationBias(layer1.size(), layer1[0].size()));
					}
					else
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						int DEPTH = layer1.size();

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

				for (int i = 0; i < output_dim; i++) {
					if (needToGenerate)
					{
						cores_set.push_back(vector<vector<vector<double>>> {});
						int DEPTH = layer2.size();
						cores_set[i] = generationCore(DEPTH, 2);

						biases_set.push_back(generationBias(layer2.size(), layer2[0].size()));
					}
					else
					{
						cores_set.push_back(vector<vector<vector<double>>> {});

						int DEPTH = layer2.size();

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

	if (needToGenerate)
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

	return 0;
}