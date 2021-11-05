#include "Matrix.hpp"

namespace PD
{
	Matrix::Matrix()
	{}
	Matrix::Matrix(size_t x, size_t y):
		x(x), y(y), matrix(x*y)
	{}
	Matrix::Matrix(const Matrix& o):
		x(o.x), y(o.y), matrix(o.matrix)
	{}
	Matrix::~Matrix()
	{}

	void Matrix::setSize(size_t x, size_t y)
	{
		this->x = x;
		this->y = y;

		matrix.resize(x*y);
	}
	size_t Matrix::getSizeX() const
	{
		return x;
	}
	size_t Matrix::getSizeY() const
	{
		return y;
	}
	void Matrix::fill(double data)
	{
		for (auto& a : matrix)
			a = data;
	}

	double* Matrix::operator[](size_t x)
	{
		return &matrix[x * this->y];
	}
	const double* Matrix::operator[](size_t x) const
	{
		return &matrix[x * this->y];
	}

	Matrix Matrix::operator+(const Matrix& r) const noexcept
	{
		Matrix result(*this);

		for (size_t i = 0; i < matrix.size(); ++i)
			result.matrix[i] += r.matrix[i];

		return result;
	}
	Matrix Matrix::operator+(double r) const noexcept
	{
		Matrix result(*this);

		for (size_t i = 0; i < matrix.size(); ++i)
			result.matrix[i] += r;

		return result;
	}
	Matrix Matrix::add(const Matrix& r) const
	{
		if (x != r.x || y != r.y)
			throw std::logic_error("Matrices of different dimensions");
		return *this + r;
	}

	Matrix Matrix::operator-(const Matrix& r) const noexcept
	{
		Matrix result(*this);

		for (size_t i = 0; i < matrix.size(); ++i)
			result.matrix[i] -= r.matrix[i];

		return result;
	}
	Matrix Matrix::operator-(double r) const noexcept
	{
		Matrix result(*this);

		for (size_t i = 0; i < matrix.size(); ++i)
			result.matrix[i] -= r;

		return result;
	}
	Matrix Matrix::sub(const Matrix& r) const
	{
		if (x != r.x || y != r.y)
			throw std::logic_error("Matrices of different dimensions");
		return *this - r;
	}

	Matrix Matrix::operator*(const Matrix& r) const noexcept
	{
		Matrix result(r.x, y);
		result.fill(0);

		for (size_t iy = 0; iy < y; iy++)
		{
			for (size_t ix = 0; ix < r.x; ix++)
			{
				for (size_t i = 0; i < x; i++)
				{
					result[ix][iy] = result[ix][iy] + (*this)[i][iy] * r[ix][i];
				}
			}
		}

		return result;
	}
	Matrix Matrix::operator*(double r) const
	{
		Matrix result(*this);
		for (auto& a : result.matrix)
			a *= r;
		return result;
	}
	Matrix Matrix::mul(const Matrix& r) const
	{
		if (x != r.y || y != r.x)
			throw std::logic_error("Matrices of the wrong dimension");
		return *this * r;
	}
}

std::ostream& operator<<(std::ostream& out, const PD::Matrix& matrix)
{
	for (size_t y = 0; y < matrix.getSizeY(); ++y)
	{
		for (size_t x = 0; x < matrix.getSizeX(); ++x)
		{
			out << matrix[x][y] << " ";
		}
		out << std::endl;
	}
	return out;
}