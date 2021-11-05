#include <iostream>
#include "Matrix.hpp"

using namespace std;
using namespace PD;

void fill(Matrix& m)
{
	for (size_t y = 0; y < m.getSizeY(); ++y)
		for (size_t x = 0; x < m.getSizeX(); ++x)
			m[x][y] = rand() % 4;
}
int main()
{
	PD::Matrix a(3, 4);
	PD::Matrix b(4, 3);
	Matrix c;

	fill(a);
	fill(b);


	c = a;
	c = a * b;

	cout << a << endl;
	cout << b << endl;
	cout << c << endl;

    Matrix d = Matrix(3, 3);
    fill(d);
    cout << d;
    cout << d.determinant() << endl;
    cout << d.inverse();
	return 0;
}