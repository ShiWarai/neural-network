vector<vector<double>> getProcessedMatrix(vector<vector<vector<double>>> matrix, vector<vector<vector<double>>> core);


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
vector<double> softmax(vector<vector<vector<double>>> a) {
	double sum = 0;
	vector<double> c;

	for (int i = 0; i < a.size(); i++)
		sum += a[i][0][0];

	if (sum == 0)
		return vector<double>{NULL};

	for (int i = 0; i < a.size(); i++)
		a[i][0][0] = a[i][0][0] / sum;

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

	return  loss / y.size();
}

// Далее идут функции последовательного вычисления производной

// der_E8 = d(loss(R)) / d(R)
double der_E8(vector<vector<vector<double>>> layerE6, int n, double solution) {
	if (softmax(layerE6) == vector<double> {NULL})
		return 0;

	return -2.0 / layerE6.size() * (solution - softmax(layerE6)[n]); // layerE7 = softmax(layerE6)
}

// der_E7 = d(softmax(R,A))/d(R)
double der_E7(vector<vector<vector<double>>> layerE6, int leader_n, int n) {

	double sums = 0;

	for (int i = 0; i < layerE6.size(); i++)
		sums += layerE6[i][0][0];

	if (leader_n == n)
		return (sums - layerE6[leader_n][0][0])/pow(sums,2);
	else
		return -(layerE6[leader_n][0][0])/pow(sums,2);
}

// der_E6 = d(relu(x))/d(x)
double der_E6(double x) {
	return x ? x >= 0 : 0;
}

// Получить производную от потери к свёрточному слою
double getLossDerivative3D(vector<vector<double>> A, vector<vector<double>> w, int j, int i) {

	if (j >= w.size() || i >= w[0].size())
		exit(0);

	double der = 0;

	vector<vector<vector<double>>> layer; // 0 - слой после расширения, 1 - слой после обработки, 2 - слой после maxpooling, 3 - слой после повторного maxpooling

	layer.push_back(matrixExpansion(A, w[0].size() - 1));

	layer.push_back(getProcessedMatrix(vector<vector<vector<double>>> { A }, vector<vector<vector<double>>> {w}));

	layer.push_back(max_pooling(layer[1]));

	layer.push_back(max_pooling(layer[2]));


	// Вычислим производную по формуле d(loss(w)) / d(w) = 
	// = (-2*b_i) / k * (solution - x(w)) * x'(w), где
	// x(w) = softmax(g(w)), sums(a,w)), а x'(w) = g'(w) * (sums - g(w)) / sums^2;
	// g(w) = relu(sum);
	// g'(w) = (sum)' * { 0: sum < 0; 1: sum >= 0;

	// Вычислим производную по формуле d(y(w)) / d(w) = d(flatten(B(w)))/d(w); 
	// B(w)x,y = maxpooling(maxpooling( C(w)x1,y1 * 4 )x,y * 4); B'(w)x,y = {1, maxI == i && maxJ == j; 0, !(maxI == i && maxJ == j)} * {1, maxI == i && maxJ == j; 0, !(maxI == i && maxJ == j)} * C'(w)x1,y1;
	// C(w)x1,y1 = sums(w * f(x,y)); C'(w)x1,y = f(x2,y2), где x2,y2 - координаты элемента A в раширенной матрице.
	// f(x,y) = matrixExpansion(A(x,y),w.size());



	return der;
}

// Получить производную от потери к однослойному вектору
double getLossDerivative2D(vector<vector<vector<double>>> layer, vector<vector<double>> w, int j, int i, double sum, int solution) {

	if (j >= w.size() || i >= w[0].size())
		exit(0);


	// Вычисление частной производной 

	double der = 0;

	vector<double> ders;

	// Вычислим производные по формуле E8' = 
	// = (-2 / k) * ((solution - Ri)
	for (int k = 0; k < w.size(); k++)
		ders.push_back(der_E8(layer, k, 1 ? solution == k : 0));

	// E7' 
	for (int k = 0; k < w.size(); k++) {
		ders[k] *= der_E7(layer, j, k);
	}

	// E6'
	der = der_E6(sum);

	double E7_sum = 0;
	for (int k = 0; k < w.size(); k++) {
		E7_sum += ders[k];
	}
	der *= E7_sum;

	return der;
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

vector<vector<double>> getProcessedMatrix(vector<vector<vector<double>>> matrix, vector<vector<vector<double>>> core) {
	vector<vector<double>> bias;

	for (int y = 0; y < matrix[0].size(); y++) {
		bias.push_back(vector<double>());
		
		for (int x = 0; x < matrix[0][0].size(); x++)
			bias[y].push_back(0);
	}

	return getProcessedMatrix(matrix, core, bias);
}

// Состоит из свёртки и сжатия
vector<vector<vector<double>>> Dense(vector<vector<vector<double>>> input, vector<vector<vector<vector<double>>>> cores_set, unsigned outputLayers,  vector<vector<vector<double>>> biases_set) {

	vector<vector<vector<double>>> layer;

	vector<vector<double>> new_matrix;

	vector<vector<vector<double>>> core;
	vector<vector<double>> bias;
	for (int i = 0; i < outputLayers; i++) {
		core = cores_set[i];
		

		if (biases_set[0][0].size() != 0) {
			bias = biases_set[i];

			new_matrix = getProcessedMatrix(input, core, bias);
		}
		else {
			new_matrix = getProcessedMatrix(input, core);
		}

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
			bias[y1][x1] = 1 - ((double)rand() * koef / (RAND_MAX));
		}
	}

	return bias;
}

// Получить весы в вектор-строке
vector<double> generationWeights(int a) {
	return generationBias(1, a, 1)[0];
}