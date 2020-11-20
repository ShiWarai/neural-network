
int max4_index(double a, double b, double c, double d) {
	int max_i;

	if (a > b && a > c && a>d) {
		max_i = 0;
	}
	else if (b > a && b > c && b > d) {
		max_i = 1;
	}
	else if (c > a && c > b && c > d) {
		max_i = 2;
	}
	else {
		max_i = 3;
	}

	return max_i;
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

vector<double> getUnitaryCode(int vectorSize, int num) {
	vector<double> a;

	for (int i = 0; i < vectorSize; i++)
		a.push_back(1 ? num == i : 0);

	return a;
}






