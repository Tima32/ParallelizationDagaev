#include <sstream>
#include <iomanip>
#include "MatrixThreadPool.hpp"
#include "ThreadPool.hpp"

static std::mutex pool_mutex;
static thread_pool pool;

namespace PD
{
	MatrixThreadPool::MatrixThreadPool()
	{}
	MatrixThreadPool::MatrixThreadPool(size_t x, size_t y) :
		x(x), y(y), matrix(x* y)
	{}
	MatrixThreadPool::MatrixThreadPool(const MatrixThreadPool& o) :
		x(o.x), y(o.y), matrix(o.matrix)
	{}
	MatrixThreadPool::~MatrixThreadPool()
	{}

	void MatrixThreadPool::setSize(size_t x, size_t y)
	{
		this->x = x;
		this->y = y;

		matrix.resize(x * y);
	}
	size_t MatrixThreadPool::getSizeX() const
	{
		return x;
	}
	size_t MatrixThreadPool::getSizeY() const
	{
		return y;
	}
	void MatrixThreadPool::fill(double data)
	{
		for (auto& a : matrix)
			a = data;
	}

	double* MatrixThreadPool::operator[](size_t x)
	{
		return &matrix[x * this->y];
	}
	const double* MatrixThreadPool::operator[](size_t x) const
	{
		return &matrix[x * this->y];
	}

	MatrixThreadPool MatrixThreadPool::operator+(const MatrixThreadPool& r) const noexcept
	{
		MatrixThreadPool result(*this);

		static const auto nt = pool.get_thread_count();
		const size_t range = size_t(matrix.size()) / nt;
		const size_t remainder = size_t(matrix.size()) % nt;

		pool_mutex.lock();
		for (uint32_t t = 0; t < nt; ++t)
		{
			pool.push_task([this, range, &result, t, &r]() {
				const size_t max = range * (t + 1);
				for (size_t i = range * t; i < max; ++i)
					result.matrix[i] += r.matrix[i];
				});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();

		const size_t max = range * nt + remainder;
		for (size_t i = range * nt; i < max; ++i)
		{
			result.matrix[i] += r.matrix[i];
		}
		return result;
	}
	MatrixThreadPool MatrixThreadPool::operator+(double r) const noexcept
	{
		MatrixThreadPool result(*this);

		static const auto nt = pool.get_thread_count();
		const size_t range = size_t(matrix.size()) / nt;
		const size_t remainder = size_t(matrix.size()) % nt;

		pool_mutex.lock();
		for (uint32_t t = 0; t < nt; ++t)
		{
			pool.push_task([this, range, &result, t, &r]() {
			const size_t max = range * (t + 1);
			for (size_t i = range * t; i < max; ++i)
				result.matrix[i] += r;
				});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();

		const size_t max = range * nt + remainder;
		for (size_t i = range * nt; i < max; ++i)
		{
			result.matrix[i] += r;
		}

		return result;
	}
	MatrixThreadPool MatrixThreadPool::add(const MatrixThreadPool& r) const
	{
		if (x != r.x || y != r.y)
			throw std::logic_error("Matrices of different dimensions");
		return *this + r;
	}

	MatrixThreadPool MatrixThreadPool::operator*(const MatrixThreadPool& r) const noexcept
	{
		MatrixThreadPool result(r.x, y);
		result.fill(0);

		pool_mutex.lock();
		for (size_t iy = 0; iy < y; iy++)
		{
			pool.push_task([this, &r, &result, iy]() {
				for (size_t ix = 0; ix < r.x; ix++)
				{
					for (size_t i = 0; i < x; i++)
					{
						result[ix][iy] = result[ix][iy] + (*this)[i][iy] * r[ix][i];
					}
				}
			});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();

		return result;
	}
	MatrixThreadPool MatrixThreadPool::operator*(double r) const
	{
		MatrixThreadPool result(*this);

		static const auto nt = pool.get_thread_count();
		const size_t range = size_t(matrix.size()) / nt;
		const size_t remainder = size_t(matrix.size()) % nt;

		pool_mutex.lock();
		for (uint32_t t = 0; t < nt; ++t)
		{
			pool.push_task([this, &result, r, range, t]() {
				const size_t max = range * (t + 1);
				for (size_t i = range * t; i < max; ++i)
					result.matrix[i] *= r;
				});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();

		const size_t max = range * nt + remainder;
		for (size_t i = range * nt; i < max; ++i)
		{
			result.matrix[i] *= r;
		}
		return result;
	}
	const MatrixThreadPool& MatrixThreadPool::operator*=(double r)
	{
		static const auto nt = pool.get_thread_count();
		const size_t range = size_t(matrix.size()) / nt;
		const size_t remainder = size_t(matrix.size()) % nt;

		pool_mutex.lock();
		for (uint32_t t = 0; t < nt; ++t)
		{
			pool.push_task([this, range, t, r]() {
				const size_t max = range * (t + 1);
				for (size_t i = range * t; i < max; ++i)
					matrix[i] *= r;
				});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();

		const size_t max = range * nt + remainder;
		for (size_t i = range * nt; i < max; ++i)
		{
			matrix[i] *= r;
		}
		return *this;
	}
	MatrixThreadPool MatrixThreadPool::mul(const MatrixThreadPool& r) const
	{
		if (x != r.y || y != r.x)
			throw std::logic_error("Matrices of the wrong dimension");
		return *this * r;
	}

	MatrixThreadPool MatrixThreadPool::transpose() const
	{
		MatrixThreadPool other(this->y, this->x);
		
		pool_mutex.lock();
		for (size_t i = 0; i < other.getSizeX(); ++i) {
			pool.push_task([this, &other, i](){
				for (size_t j = 0; j < other.getSizeY(); ++j) {
					other[i][j] = (*this)[j][i];
				}
			});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();
		return other;
	}

	MatrixThreadPool MatrixThreadPool::getMinor(const size_t n, const size_t m) const
	{
		if ((this->x == 1) || (this->y == 1)) {
			throw std::logic_error("Y and X should be more than 1");
		}
		if ((this->x < n) || (1 > n) || (this->y < m) || (1 > m)) {
			throw std::logic_error("Wrong n or m");
		}
		MatrixThreadPool minor(this->x - 1, this->y - 1);
		bool flag_row = false;
		bool flag_col = false;

		for (size_t i = 0; i < minor.getSizeX(); ++i) {
			for (size_t j = 0; j < minor.getSizeY(); ++j) {
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
	double MatrixThreadPool::determinant() const
	{
		if (this->x != this->y) {
			throw std::logic_error("MatrixThreadPool should be square");
		}
		double d = 0;
		if (this->x == 1) {
			return (*this)[0][0];
		}

		pool_mutex.lock();
		std::mutex dm;
		for (size_t i = 0; i < this->x; ++i) {
			pool.push_task([this, &d, i, &dm]() {
				size_t k = i % 2 ? -1 : 1;
				auto r = k * (*this)[i][0] * this->getMinor(i + 1, 1).determinantPr();

				dm.lock();
				d += r;
				dm.unlock();
			});
		}
		pool.wait_for_tasks();
		pool_mutex.unlock();
		return d;
	}

	//private
	double MatrixThreadPool::determinantPr() const
	{
		if (this->x != this->y) {
			throw std::logic_error("MatrixThreadPool should be square");
		}
		double d = 0;
		size_t k = 1;
		if (this->x == 1) {
			return (*this)[0][0];
		}
		for (size_t i = 0; i < this->x; ++i) {
			d += k * (*this)[i][0] * this->getMinor(i + 1, 1).determinantPr();
			k *= -1;
		}
		return d;
	}
}

std::ostream& operator<<(std::ostream& out, const PD::MatrixThreadPool& matrix)
{
	size_t max_len{ 0 };
	std::stringstream ss;
	for (size_t y = 0; y < matrix.getSizeY(); ++y)
	{
		for (size_t x = 0; x < matrix.getSizeX(); ++x)
		{
			ss << matrix[x][y];
			auto str = ss.str();
			auto len = size_t(str.size());
			ss.str("");
			max_len = len > max_len ? len : max_len;
		}
	}

	for (size_t y = 0; y < matrix.getSizeY(); ++y)
	{
		for (size_t x = 0; x < matrix.getSizeX(); ++x)
		{
			out << std::setw(max_len) << matrix[x][y] << " ";
		}
		out << std::endl;
	}
	return out;
}