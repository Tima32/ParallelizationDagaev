#pragma once
#include <iostream>
#include <vector>

namespace PD
{
	class Matrix
	{
	public:
		Matrix();
		Matrix(size_t x, size_t y);
		Matrix(const Matrix& o);
		~Matrix();

		void setSize(size_t x, size_t y);
		size_t getSizeX() const;
		size_t getSizeY() const;
		void fill(double data);

		double* operator[](size_t x);
		const double* operator[](size_t x) const;

		Matrix operator+(const Matrix& r) const noexcept;
		Matrix operator+(double r) const noexcept;
		Matrix add(const Matrix& r) const; //exception

		Matrix operator-(const Matrix& r) const noexcept;
		Matrix operator-(double r) const noexcept;
		Matrix sub(const Matrix& r) const; //exception

		Matrix operator*(const Matrix& r) const noexcept;
		Matrix operator*(double r) const;
		const Matrix& operator*=(double r);
		Matrix mul(const Matrix& r) const; //exception

        Matrix transpose() const;

        Matrix getMinor(const size_t n, const size_t m) const;
        double determinant() const;

        Matrix inverse() const;

	private:
		size_t x{0};
		size_t y{0};
		std::vector<double> matrix;
	};
}

std::ostream& operator<<(std::ostream& out, const PD::Matrix& matrix);