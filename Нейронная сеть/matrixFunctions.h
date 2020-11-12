// �������� ������� ������� Y x X
vector<vector<double>> createFilledVector(int y, int x) {
	vector<vector<double>> c;

	for (int y1 = 0; y1 < y; y1++) {
		c.push_back(vector<double> {});
		for (int x1 = 0; x1 < x; x1++)
			c[y1].push_back(0);
	}

	return c;
}

// ������� �� matrix ������������� AxB � ��������� y0, x0 (�������� ������!!!)
vector<vector<double>> matrixSlicer(vector<vector<double>> matrix, unsigned y0, unsigned x0, unsigned a, unsigned b) {
	auto processed_matrix = createFilledVector(a, b);

	for (int y = 0; y < a; y++) {
		for (int x = 0; x < b; x++) {
			processed_matrix[y][x] = matrix[y0 + y][x0 + x];
		}
	}

	return processed_matrix;
}

// a * b = c (������������ ������������)
vector<vector<double>> dot(vector<vector<double>> a, vector<vector<double>> b) {

	if ((a.size() != b.size()) && (a[0].size() != b[0].size())) {
		cout << "������� �������� �� ���������" << endl;
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


// a + b = c (������������ ��������)
vector<vector<double>> sumElements(vector<vector<double>> a, vector<vector<double>> b) {

	if ((a.size() != b.size()) && (a[0].size() != b[0].size())) {
		cout << "������� �������� �� ���������" << endl;
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


// ����� ���� ���������
double elementsSum(vector<vector<double>> a) {
	double sum = 0;
	for (int y1 = 0; y1 < a.size(); y1++) {
		for (int x1 = 0; x1 < a[0].size(); x1++) {
			sum += a[y1][x1];
		}
	}

	return sum;
}


// ��������� ����������� �������
vector<vector<double>> matrixExpansion(vector<vector<double>> matrix, unsigned padding) {

	auto new_matrix = createFilledVector(matrix.size() + padding, matrix[0].size() + padding);
	int max_panding_1 = (int)ceil((double)padding / 2); // ������-�����
	int max_panding_2 = (int)floor((double)padding / 2); //���-������

	int currentX, currentY;

	// ���������� ����������
	for (int padd_ = 1; padd_ <= padding; padd_++) {

		// ��������� ������
		int panding = (int)ceil((double)padd_ / 2) - 1;

		// ��� ������-������
		if (padd_ % 2 == 1)
		{
			// �����

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

			// �����

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
		// ��� ���-�������
		else
		{
			// ��
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

			// ������
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

	// ���������� ������
	for (int y = 0; y < matrix.size(); y++) {
		for (int x = 0; x < matrix[0].size(); x++) {
			new_matrix[y + max_panding_1][x + max_panding_1] = matrix[y][x];
		}
	}

	return new_matrix;

}