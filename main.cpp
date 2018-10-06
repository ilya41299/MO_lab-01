#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

class Simplex {
	double** table;
	int n, m; // n - кол-во строк. m - Число столбцов c учётом ст. свободных членов и строки функции

	void print(); // вывод матрицы на экран
	void Finding_the_optimal_solution(); // поиск оптимального решения
	void check_flag(bool& flag); // проверка решения на оптимальность и возможность
	int result_column(); // поиск результирующего столбца
	int result_row(unsigned res_col); //поиск результирующей строки
	void make_table(unsigned res_row, unsigned res_col); // преобразование таблицы с помощью жордановых исключений
	void Find_reference_solution(); // поиск опорного решения

public:
	Simplex();
	Simplex(const std::string name);
	~Simplex();

private:
};

Simplex::Simplex(const std::string name)
{
	std::ifstream fin;
	fin.open(name.c_str());
	if (!(fin.is_open()))
		throw std::invalid_argument("An error has occured while reading input data");
	char op;
	if (fin >> m && fin >> op && op == ',' && fin >> n) {
		table = new double*[m];
		for (unsigned int i = 0; i < m; i++) {
			table[i] = new double[n];
			for (unsigned int j = 0; j < n; j++) {
				fin >> table[i][j];
			}
		}
	}
	else
		throw std::invalid_argument("An error has occured while reading input data");
	fin.close();
	std::cout << "Initial simplex table:" << std::endl;
	print();
	Finding_the_optimal_solution();
}

Simplex::Simplex()
{
	table = nullptr;
	n = 0;
	m = 0;
}

Simplex::~Simplex()
{
	for (unsigned int i = 0; i < this->n; i++) {
		delete[] this->table[i];
	}
	delete[] this->table;
	n = 0;
	m = 0;
}



void Simplex::print()
{

	for (std::size_t i = 0; i < m; ++i) {
		std::cout << '\n';
		for (std::size_t j = 0; j < n; ++j) {
			std::cout << table[i][j];
			if (j != n - 1) {
				std::cout << ' ';
			}
		}
	}
	std::cout << std::endl << std::endl;
		
}
void Simplex::Finding_the_optimal_solution()
{
	bool flag = true;
	unsigned res_col, res_row;
	Find_reference_solution();
	print();
	while (flag) {
		res_col = result_column();
		res_row = result_row(res_col);
		//swap_res_row_col(res_row, res_col);
		make_table(res_row, res_col);
		check_flag(flag);
		if(!flag) std::cout << "This is the optimal solution:" << std::endl;
		print();
	}
}

void Simplex::check_flag(bool& flag)
{
	//проверяет решение на оптимальность
	//если в столбце свободных членов все значения положительны(решение допустимо)
	//и в целевой функции  коэффициенты отрицательны
	//то решение оптимально
	for (unsigned i = 0; i < n - 2; i++) {
		if (table[i][0] < 0)
			break;
		else
			flag = false;
	}

	if (!flag) {
		for (unsigned j = 1; j < m; j++) {
			if (table[n - 1][j] >= 0) {
				flag = true;
				break;
			}
		}
	}
}

int Simplex::result_column()
{
	//если в столбце своб. чл. находим отрицательный элемент
	//то идём по этой строке в поиске рез.ст(первого эл-та с отриц знач)
	for (unsigned i = 0; i < n - 1; i++) {
		if (table[i][0] < 0) {
			for (unsigned j = 0; j < m; j++) {
				if (table[i][j] < 0) {
					return j;
				}
			}
		}
	}

	//если такой элемент не был найден, то
	//в строке целевой функции(n-1ой) ищем неотрицй.эл-т
	//после чего в этом столбце ищем хотябы один неотрицательный элемент
	// в противном случае - ЦФ не ограничена
	for (unsigned j = 1; j < m; j++) {
		if (table[n - 1][j] >= 0) {
			for (unsigned i = 0; i < n - 2; i++) {
				if (table[i][j] > 0)
					return j;
			}
			throw std::logic_error("Целевая ф-я не ограничена снизу");
		}
	}
}

int Simplex::result_row(unsigned res_col)
{
	//number_1 - инициализируем 0. Он будет хранить минимальное значение
	//number_2 - временная переменная, применяемая для сравнения и поиска минимума
	unsigned res_row;
	double number_1, number_2;
	number_1 = 0;
	for (unsigned i = 0; i < n - 1; i++) {
		if (table[i][res_col] == 0)
			continue;
		number_2 = table[i][0] / table[i][res_col];
		if (number_2 > 0)
			if (number_1 == 0) {
				number_1 = number_2;
				res_row = i;
			}
			else if (number_2 < number_1) {
				number_1 = number_2;
				res_row = i;
			}
	}
	return res_row;
}

void Simplex::Find_reference_solution()
{
	bool valid = false;
	for (unsigned i = 0; i < n - 2; i++) {
		if (table[i][0] < 0)
			break;
		else
			valid = true;
	}
	if (valid) std::cout << "This reference solution:" << std::endl;
	else 
	{
		unsigned res_col = result_column();
		unsigned res_row = result_row(res_col);
		make_table(res_row, res_col);
	}
}

void Simplex::make_table(unsigned res_row, unsigned res_col)
{
	//new_table - доп симплекс таблица
	double** new_table = new double*[n];
	new_table = new double*[m];
	for (unsigned int i = 0; i < m; i++) {
		new_table[i] = new double[n];
		for (unsigned int j = 0; j < n; j++) {
			new_table[i][j] = table[i][j];
		}
	}

	table[res_row][res_col] = 1 / new_table[res_row][res_col];

	for (unsigned i = 0; i < n; i++) {
		for (unsigned j = 0; j < m; j++) {
			if (i == res_row && j == res_col)
				continue;
			else if (i == res_row) {
				table[i][j] = new_table[i][j] / new_table[res_row][res_col];
			}
			else if (j == res_col) {
				table[i][j] = -new_table[i][j] / new_table[res_row][res_col];
			}
			else if (i != res_row && j != res_col) {
				table[i][j] = new_table[i][j] - ((new_table[res_row][j] * new_table[i][res_col]) / new_table[res_row][res_col]);
			}
		}
	}

	//Очищаем выделенную память под матрицу new_table
	for (auto i = 0; i < n; i++) {
		delete[] new_table[i];
	}
	delete[] new_table;
}

int main()
{
	Simplex A("test.txt");
	return 0;
}
