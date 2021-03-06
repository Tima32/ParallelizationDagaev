#pragma once
#include <iostream>
#include <vector>

//# ifdef _OPENMP
namespace PD
{
	class MatrixOpenMP
	{
	public:
		MatrixOpenMP();
		MatrixOpenMP(int64_t x, int64_t y);
		MatrixOpenMP(const MatrixOpenMP& o);
		~MatrixOpenMP();

		void setSize(int64_t x, int64_t y);
		int64_t getSizeX() const;
		int64_t getSizeY() const;
		void fill(double data);

		double* operator[](int64_t x);
		const double* operator[](int64_t x) const;

		MatrixOpenMP operator+(const MatrixOpenMP& r) const noexcept;
		MatrixOpenMP operator+(double r) const noexcept;
		MatrixOpenMP add(const MatrixOpenMP& r) const; //exception

		MatrixOpenMP operator*(const MatrixOpenMP& r) const noexcept;
		MatrixOpenMP operator*(double r) const;
		const MatrixOpenMP& operator*=(double r);
		MatrixOpenMP mul(const MatrixOpenMP& r) const; //exception

		MatrixOpenMP transpose() const;

		MatrixOpenMP getMinor(const int64_t n, const int64_t m) const;
		double determinant() const;

	private:
		int64_t x{ 0 };
		int64_t y{ 0 };
		std::vector<double> matrix;

		double determinantPr() const;
	};
}

std::ostream& operator<<(std::ostream& out, const PD::MatrixOpenMP& matrix);
//# endif
