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

// Свёртка к максимальному (выборка из 2x2)
vector<vector<vector<double>>> max_pooling(vector<vector<double>> a) {
	vector<vector<double>> finalBuffer;
	int currentSize = a.size();

	vector<vector<double>> max_poses;

	for (int y = 0; y < currentSize; y += 2) {
		vector<double> buffer;
		vector<double> buffer_poses;

		for (int x = 0; x < currentSize; x += 2) {

			vector <double> cs = max4(a[y][x], a[y][x + 1], a[y + 1][x], a[y + 1][x + 1]);

			buffer.push_back(cs[0]);
			buffer_poses.push_back(cs[1]);

			a[y][x] = 0;
			a[y][x + 1] = 0;
			a[y + 1][x] = 0;
			a[y + 1][x + 1] = 0;
		}

		finalBuffer.push_back(buffer);
		max_poses.push_back(buffer_poses);
	}

	/*
	for (int y = 0; y < currentSize / 2; y++) {
		for (int x = 0; x < currentSize / 2; x++) {
			a[y][x] = finalBuffer[y][x];
			cout << setw(6) << setprecision(4) << a[y][x] << " ";
		}
		cout << endl;
	} */

	return vector<vector<vector<double>>> {finalBuffer, max_poses};
}

// Операция обратная max_pooling для выборки 2x2
vector<vector<double>> reverse_max_pooling(vector<vector<double>> a, vector<vector<double>> max_poses) {

	if (a.size() != max_poses.size() || a[0].size() != max_poses[0].size())
		return vector<vector<double>> { NULL };

	int size_new_y = a.size() * 2;
	int size_new_x = a[0].size() * 2;

	vector<vector<double>> b;

	for (int y = 0; y < size_new_y; y ++) {
		b.push_back(vector<double>());
		for (int x = 0; x < size_new_x; x ++) {
			b[y].push_back(0);
		}
	}

	for (int y = 0; y < size_new_y; y+=2) {
		for (int x = 0; x < size_new_x; x+=2) {
			if (max_poses[y / 2][x / 2] == 0)
				b[y][x] = a[y / 2][x / 2];
			else if (max_poses[y / 2][x / 2] == 1)
				b[y][x + 1] = a[y / 2][x / 2];
			else if (max_poses[y / 2][x / 2] == 2)
				b[y + 1][x] = a[y / 2][x / 2];
			else
				b[y + 1][x + 1] = a[y / 2][x / 2];
		}
	}

	return b;
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

// Обратная развёртка (не является обратной flatten, ибо имеет др. размерность на выходе)
vector<vector<vector<double>>> reverse_flatten(vector<double> a, int dim1, int dim2, int dim3) {
	vector<vector<vector<double>>> b;

	// Заполняем массив
	for (int z = 0; z < dim1; z++) {
		b.push_back(vector<vector<double>>{});
		for (int y = 0; y < dim2; y++) {
			b[z].push_back(vector<double> {});
		}
	}

	for (int i = 0; i < a.size(); i++)
		b[i / (dim2 * dim3)][(i % (dim2 * dim3)) / dim3].push_back(a[i]);

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
double der_loss(vector<vector<vector<double>>> layerE6, int n, double solution) {
	if (softmax(layerE6) == vector<double> {NULL})
		return 0;

	return -2.0 / layerE6.size() * (solution - softmax(layerE6)[n]); // layerE7 = softmax(layerE6)
}

// der_E7 = d(softmax(R,A))/d(R)
double der_softmax(vector<vector<vector<double>>> layerE6, int leader_n, int n, double sums) {


	if (leader_n == n)
		return (sums - layerE6[leader_n][0][0])/pow(sums,2);
	else
		return -(layerE6[leader_n][0][0])/pow(sums,2);
}

// der_E6 = d(relu(x))/d(x)
double der_relu(double x) {
	return x ? x > 0 : (0);
}

// der_E4 = d(W*X*E6_x..)/dX
vector<double> ders_E4(vector<vector<double>> W, vector<double> E6_x) {
	vector<double> ders;

	double sum;
	for (int i = 0; i < W[0].size(); i++) {
		sum = 0;
		for (int j = 0; j < E6_x.size(); j++)
			sum += (W[j][i] * E6_x[j]);
		ders.push_back(sum);
	}
	return ders;
}


// Обратная свёртка (производные для ядер 2x2)
vector<vector<vector<double>>> ders_cores(vector<vector<double>> input, vector<vector<vector<double>>> ders_E1) {

	const unsigned core_size = 2;
	const unsigned y_size = input.size();
	const unsigned x_size = input[0].size();

	vector<vector<vector<double>>> ders_cores_; // Производные ядер
	unsigned outputLayers = ders_E1.size();

	int panding_ = (int)ceil((double)(core_size - 1) / 2); // Определяем начало гл. массива
	vector<vector<double>> flat = matrixExpansion(input, panding_); // Получаем расширенную матрицу плоскости

		for (int k = 0; k < outputLayers; k++) {
		vector<vector<double>> der_core = createFilledVector(core_size, core_size);

			for (int y = 0; y < y_size; y++) {
				for (int x = 0; x < x_size; x++) {

					// Производим умножение производной следующего слоя на текущее значение X

					der_core[0][0] += flat[y + panding_ - 1][x + panding_ - 1] * ders_E1[k][y][x];
					der_core[0][1] += flat[y + panding_ - 1][x + panding_] * ders_E1[k][y][x];
					der_core[1][0] += flat [y + panding_] [x + panding_ - 1] * ders_E1[k][y][x];
					der_core[1][1] += flat[y + panding_][x + panding_] * ders_E1[k][y][x];

				}
			}		

		ders_cores_.push_back(der_core);
	}

	return ders_cores_;
}


// Получить производную от потери к свёрточному слою
double getLossDerivative3D(vector<vector<double>> A, vector<vector<double>> w, int j, int i) {

	if (j >= w.size() || i >= w[0].size())
		exit(0);

	double der = 0;

	vector<vector<vector<double>>> layer; // 0 - слой после расширения, 1 - слой после обработки, 2 - слой после maxpooling, 3 - слой после повторного maxpooling

	layer.push_back(matrixExpansion(A, w[0].size() - 1));

	layer.push_back(getProcessedMatrix(vector<vector<vector<double>>> { A }, vector<vector<vector<double>>> {w}));

	// layer.push_back(max_pooling(layer[1]));

	// layer.push_back(max_pooling(layer[2]));


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
double getLossDerivative2D(vector<vector<vector<double>>> layer, vector<vector<double>> w, int j,  double sum, double sums, int solution) {

	if ( j >= w.size() )
		exit(0);


	// Вычисление частной производной 

	double der = 0;

	vector<double> ders;

	// Вычислим производные по формуле E8' = 
	// = (-2 / k) * ((solution - Ri)
	for (int k = 0; k < w.size(); k++)
		ders.push_back(der_loss(layer, k, 1 ? solution == k : 0));

	// E7' 
	for (int k = 0; k < w.size(); k++) {
		ders[k] *= der_softmax(layer, j, k, sums);
	}

	// E6'
	der = der_relu(sum);

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
				core[i][y1][x1] = 1 - ((double)rand() / (RAND_MAX));
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