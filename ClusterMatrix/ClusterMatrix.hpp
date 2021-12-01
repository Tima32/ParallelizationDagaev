#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <SFML/Network.hpp>
#include "ThreadPool.hpp"

namespace PD
{
	class ClusterMatrix;
}

sf::Packet& operator<<(sf::Packet& packet, const PD::ClusterMatrix& matrix);
sf::Packet& operator>>(sf::Packet& packet, PD::ClusterMatrix& matrix);

namespace PD
{
	class ClusterMatrix
	{
	public:
		static std::mutex pool_mutex;
		static thread_pool pool;

		ClusterMatrix();
		ClusterMatrix(size_t x, size_t y);
		ClusterMatrix(const ClusterMatrix& o);
		~ClusterMatrix();

		void setSize(size_t x, size_t y);
		size_t getSizeX() const;
		size_t getSizeY() const;
		void fill(double data);

		double* operator[](size_t x);
		const double* operator[](size_t x) const;

		ClusterMatrix operator+(const ClusterMatrix& r) const noexcept;
		ClusterMatrix operator+(double r) const noexcept;
		ClusterMatrix add(const ClusterMatrix& r) const; //exception

		ClusterMatrix operator*(const ClusterMatrix& r) const noexcept;
		ClusterMatrix operator*(double r) const;
		const ClusterMatrix& operator*=(double r);
		ClusterMatrix mul(const ClusterMatrix& r) const; //exception
		ClusterMatrix mul(const ClusterMatrix& r, size_t y_line) const;

		ClusterMatrix transpose() const;

		ClusterMatrix getMinor(const size_t n, const size_t m) const;
		double determinant() const;

	private:
		size_t x{ 0 };
		size_t y{ 0 };
		std::vector<double> matrix;

		double determinantPr() const;

		friend sf::Packet& ::operator<<(sf::Packet&, const PD::ClusterMatrix&);
		friend sf::Packet& ::operator>>(sf::Packet&, PD::ClusterMatrix&);
	};
}

std::ostream& operator<<(std::ostream& out, const PD::ClusterMatrix& matrix);