#pragma once
#include <iostream>
#include <vector>

namespace PD
{
	class MatrixThreadPool
	{
	public:
		MatrixThreadPool();
		MatrixThreadPool(size_t x, size_t y);
		MatrixThreadPool(const MatrixThreadPool& o);
		~MatrixThreadPool();

		void setSize(size_t x, size_t y);
		size_t getSizeX() const;
		size_t getSizeY() const;
		void fill(double data);

		double* operator[](size_t x);
		const double* operator[](size_t x) const;

		MatrixThreadPool operator+(const MatrixThreadPool& r) const noexcept;
		MatrixThreadPool operator+(double r) const noexcept;
		MatrixThreadPool add(const MatrixThreadPool& r) const; //exception

		MatrixThreadPool operator*(const MatrixThreadPool& r) const noexcept;
		MatrixThreadPool operator*(double r) const;
		const MatrixThreadPool& operator*=(double r);
		MatrixThreadPool mul(const MatrixThreadPool& r) const; //exception

		MatrixThreadPool transpose() const;

		MatrixThreadPool getMinor(const size_t n, const size_t m) const;
		double determinant() const;

	private:
		size_t x{ 0 };
		size_t y{ 0 };
		std::vector<double> matrix;

		double determinantPr() const;
	};
}

std::ostream& operator<<(std::ostream& out, const PD::MatrixThreadPool& matrix);