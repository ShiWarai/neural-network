
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
					double der = getLossDerivative2D(layer4, weights, weightJ, weightI, 1 ? weightJ == answer : 0);
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
