#include <sstream>
#include <iomanip>
#include <omp.h>
#include "MatrixOpenMP.hpp"

namespace PD
{
	MatrixOpenMP::MatrixOpenMP()
	{}
	MatrixOpenMP::MatrixOpenMP(int64_t x, int64_t y) :
		x(x), y(y), matrix(x * y)
	{}
	MatrixOpenMP::MatrixOpenMP(const MatrixOpenMP& o) :
		x(o.x), y(o.y), matrix(o.matrix)
	{}
	MatrixOpenMP::~MatrixOpenMP()
	{}

	void MatrixOpenMP::setSize(int64_t x, int64_t y)
	{
		this->x = x;
		this->y = y;

		matrix.resize(x * y);
	}
	int64_t MatrixOpenMP::getSizeX() const
	{
		return x;
	}
	int64_t MatrixOpenMP::getSizeY() const
	{
		return y;
	}
	void MatrixOpenMP::fill(double data)
	{
		for (auto& a : matrix)
			a = data;
	}

	double* MatrixOpenMP::operator[](int64_t x)
	{
		return &matrix[x * this->y];
	}
	const double* MatrixOpenMP::operator[](int64_t x) const
	{
		return &matrix[x * this->y];
	}

	MatrixOpenMP MatrixOpenMP::operator+(const MatrixOpenMP& r) const noexcept
	{
		MatrixOpenMP result(*this);

		static const auto nt = omp_get_max_threads();
		const int64_t range = int64_t(matrix.size()) / nt;
		const int64_t remainder = int64_t(matrix.size()) % nt;

#pragma omp parallel for
		for (int t = 0; t < nt; ++t)
		{
			const int64_t max = range * (t + 1);
			for (int64_t i = range * t; i < max; ++i)
				result.matrix[i] += r.matrix[i];
		}

		const int64_t max = range * nt + remainder;
		for (int64_t i = range * nt; i < max; ++i)
		{
			result.matrix[i] += r.matrix[i];
		}
		return result;
	}
	MatrixOpenMP MatrixOpenMP::operator+(double r) const noexcept
	{
		MatrixOpenMP result(*this);

		static const auto nt = omp_get_max_threads();
		const int64_t range = int64_t(matrix.size()) / nt;
		const int64_t remainder = int64_t(matrix.size()) % nt;

#pragma omp parallel for
		for (int t = 0; t < nt; ++t)
		{
			const int64_t max = range * (t + 1);
			for (int64_t i = range * t; i < max; ++i)
				result.matrix[i] += r;
		}

		const int64_t max = range * nt + remainder;
		for (int64_t i = range * nt; i < max; ++i)
		{
			result.matrix[i] += r;
		}

		return result;
	}
	MatrixOpenMP MatrixOpenMP::add(const MatrixOpenMP& r) const
	{
		if (x != r.x || y != r.y)
			throw std::logic_error("Matrices of different dimensions");
		return *this + r;
	}

	MatrixOpenMP MatrixOpenMP::operator*(const MatrixOpenMP& r) const noexcept
	{
		MatrixOpenMP result(r.x, y);
		result.fill(0);

#pragma omp parallel for
		for (int64_t iy = 0; iy < y; iy++)
		{
			for (int64_t ix = 0; ix < r.x; ix++)
			{
				for (int64_t i = 0; i < x; i++)
				{
					result[ix][iy] = result[ix][iy] + (*this)[i][iy] * r[ix][i];
				}
			}
		}

		return result;
	}
	MatrixOpenMP MatrixOpenMP::operator*(double r) const
	{
		MatrixOpenMP result(*this);
		
		static const auto nt = omp_get_max_threads();
		const int64_t range = int64_t(matrix.size()) / nt;
		const int64_t remainder = int64_t(matrix.size()) % nt;

#pragma omp parallel for
		for (int t = 0; t < nt; ++t)
		{
			const int64_t max = range * (t + 1);
			for (int64_t i = range * t; i < max; ++i)
				result.matrix[i] *= r;
		}

		const int64_t max = range * nt + remainder;
		for (int64_t i = range * nt; i < max; ++i)
		{
			result.matrix[i] *= r;
		}
		return result;
	}
	const MatrixOpenMP& MatrixOpenMP::operator*=(double r)
	{
		static const auto nt = omp_get_max_threads();
		const int64_t range = int64_t(matrix.size()) / nt;
		const int64_t remainder = int64_t(matrix.size()) % nt;

#pragma omp parallel for
		for (int t = 0; t < nt; ++t)
		{
			const int64_t max = range * (t + 1);
			for (int64_t i = range * t; i < max; ++i)
				matrix[i] *= r;
		}

		const int64_t max = range * nt + remainder;
		for (int64_t i = range * nt; i < max; ++i)
		{
			matrix[i] *= r;
		}
		return *this;
	}
	MatrixOpenMP MatrixOpenMP::mul(const MatrixOpenMP& r) const
	{
		if (x != r.y || y != r.x)
			throw std::logic_error("Matrices of the wrong dimension");
		return *this * r;
	}

	MatrixOpenMP MatrixOpenMP::transpose() const
	{
		MatrixOpenMP other(this->y, this->x);
#pragma omp parallel for
		for (int64_t i = 0; i < other.getSizeX(); ++i) {
			for (int64_t j = 0; j < other.getSizeY(); ++j) {
				other[i][j] = (*this)[j][i];
			}
		}
		return other;
	}

	MatrixOpenMP MatrixOpenMP::getMinor(const int64_t n, const int64_t m) const
	{
		if ((this->x == 1) || (this->y == 1)) {
			throw std::logic_error("Y and X should be more than 1");
		}
		if ((this->x < n) || (1 > n) || (this->y < m) || (1 > m)) {
			throw std::logic_error("Wrong n or m");
		}
		MatrixOpenMP minor(this->x - 1, this->y - 1);
		bool flag_row = false;
		bool flag_col = false;

		for (int64_t i = 0; i < minor.getSizeX(); ++i) {
			for (int64_t j = 0; j < minor.getSizeY(); ++j) {
				if (i == n - 1) {
					flag_row = true;
				}
				if (j == m - 1) {
					flag_col = true;
				}
				minor[i][j] = (*this)[i + flag_row][j + flag_col];
			}
			flag_col = false;
		}
		return minor;
	}
	double MatrixOpenMP::determinant() const
	{
		if (this->x != this->y) {
			throw std::logic_error("MatrixOpenMP should be square");
		}
		double d = 0;
		int64_t k = 1;
		if (this->x == 1) {
			return (*this)[0][0];
		}
#pragma omp parallel for
		for (int64_t i = 0; i < this->x; ++i) {
			d += k * (*this)[i][0] * this->getMinor(i + 1, 1).determinantPr();
			k *= -1;
		}
		return d;
	}

	//private
	double MatrixOpenMP::determinantPr() const
	{
		if (this->x != this->y) {
			throw std::logic_error("MatrixOpenMP should be square");
		}
		double d = 0;
		int64_t k = 1;
		if (this->x == 1) {
			return (*this)[0][0];
		}
		for (int64_t i = 0; i < this->x; ++i) {
			d += k * (*this)[i][0] * this->getMinor(i + 1, 1).determinantPr();
			k *= -1;
		}
		return d;
	}
}

std::ostream& operator<<(std::ostream& out, const PD::MatrixOpenMP& matrix)
{
	int64_t max_len{ 0 };
	std::stringstream ss;
	for (int64_t y = 0; y < matrix.getSizeY(); ++y)
	{
		for (int64_t x = 0; x < matrix.getSizeX(); ++x)
		{
			ss << matrix[x][y];
			auto str = ss.str();
			auto len = int64_t(str.size());
			ss.str("");
			max_len = len > max_len ? len : max_len;
		}
	}

	for (int64_t y = 0; y < matrix.getSizeY(); ++y)
	{
		for (int64_t x = 0; x < matrix.getSizeX(); ++x)
		{
			out << std::setw(max_len) << matrix[x][y] << " ";
		}
		out << std::endl;
	}
	return out;
}