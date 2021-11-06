#include <iostream>
#include <iomanip>
#include <random> // для std::random_device и std::mt19937
#include <chrono>
#include <fstream>
#include "Matrix.hpp"

using namespace std;
using namespace PD;

void Fill(Matrix& m)
{
	for (size_t y = 0; y < m.getSizeY(); ++y)
		for (size_t x = 0; x < m.getSizeX(); ++x)
			m[x][y] = double(x * 10 + y);
}
template<typename T> void FillRandom(T& m)
{
	static std::random_device rd;
	static std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом
	static std::uniform_int_distribution<uint64_t> distribution(1, 4);

	for (size_t y = 0; y < m.getSizeY(); ++y)
		for (size_t x = 0; x < m.getSizeX(); ++x)
			m[x][y] = double(distribution(mersenne));
}

template<typename T> void TestTemplate(size_t multipler = 10, size_t iterations = 10000)
{
	wfstream test_file(L"Test.txt", ios::out);
	test_file.precision(5);

	for (size_t i = 0; i < iterations; ++i)
	{
		size_t size = 3 + i;
		T original(size, size);
		FillRandom(original);

		auto begin_t = std::chrono::high_resolution_clock::now();

		auto inv = original.inverse();
		auto result = inv * original;

		auto end_t = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t);
		cout << "Time: " << setprecision(5) << (double)duration.count() / 1000 << endl;
		test_file << (double)duration.count() / 1000 << " ";

		cout << i << endl;
	}
	test_file.close();
}
int main()
{
	std::cout << std::fixed << std::setprecision(2);

	TestTemplate<Matrix>();

	/*PD::Matrix a(3, 4);
	PD::Matrix b(4, 3);
	Matrix c;

	FillRandom(a);
	FillRandom(b);

	c = a;
	c = a * b;

	cout << a << endl;
	cout << b << endl;
	cout << c << endl;

    Matrix d = Matrix(3, 3);
	FillRandom(d);
    cout << d;
    cout << d.determinant() << endl;
    cout << d.inverse();
	cout << endl;
	auto inv = d.inverse();
	cout << inv * d << endl;
	cout << endl;
	cout << d * inv << endl;*/

	return 0;
}