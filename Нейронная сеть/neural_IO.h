

// Запись матрицы в файл
void writeMatrixInFile(ofstream& fout, vector<vector<double>> matrix)
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
vector<vector<double>> readMatrixFromFile(ifstream& fin)
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
