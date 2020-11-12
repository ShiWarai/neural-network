
// Поэлементный максимум
vector<vector<double>> reluFunction(vector<vector<double>> a) {

	for (int y1 = 0; y1 < a.size(); y1++) {
		for (int x1 = 0; x1 < a[0].size(); x1++) {
			a[y1][x1] = max(a[y1][x1], 0);
		}
	}

	return a;
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

// Вычисление потерь
double getLoss(vector<double> y, vector<double> solution) {
	if (y.size() != solution.size())
		exit(0);

	double loss = 0;

	for (int i = 0; i < y.size(); i++)
		loss += pow(solution[i] - y[i], 2);

	return  loss / (double)y.size();
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

	double g = max(sum, 0);

	// x'(w) = g'(w) * (sums - g(w)) / sums^2

	double d_softmax = d_g * (sums - g) / pow(sums, 2);

	return ((-2 * a[i]) / w.size()) * (solution - softmax_result) * d_softmax;
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
			for (int x = 0; x < x_size; x++) {

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
		if (do_max_pooling)
			new_matrix = max_pooling(new_matrix);

		// Добавляем уже сжатый слой
		layer.push_back(new_matrix);
	}

	return layer;
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
				core[i][y1][x1] = 1 - ((double)rand() / (RAND_MAX)) * 2;
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
			bias[y1][x1] = -0.5 + ((double)rand() * koef / (RAND_MAX));
		}
	}

	return bias;
}

// Получить весы в вектор-строке
vector<double> generationWeights(int a) {
	return generationBias(1, a, 2)[0];
}