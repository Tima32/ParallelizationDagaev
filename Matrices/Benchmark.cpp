#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <iomanip>
#include <random> // для std::random_device и std::mt19937
#include <chrono>
#include <fstream>
#include <codecvt>

#include "Matrix.hpp"
#include "MatrixOpenMP.hpp"

using namespace std;

namespace PD::BENCH
{
	std::wstring utf8ToWstring(const std::string& str) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.from_bytes(str);
	}
	std::wstring DeleteCharacters(const std::wstring& str)
	{
		wstring r;
		r.reserve(str.size() + 1);

		for (auto w : str)
			if (w != L':')
				r += w;

		return r;
	}

	template<typename T> void Fill(T& m)
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
	template<typename T> double Sum(const T& m)
	{
		double r{ 0 };
		for (size_t y = 0; y < m.getSizeY(); ++y)
			for (size_t x = 0; x < m.getSizeX(); ++x)
				r += m[x][y];
		return r;
	}

	template<typename T> void TestMulScalar(size_t multipler = 100, size_t iterations = 50)
	{
		static const wstring func_name{ L"TestMulScalar" };


		static const wstring test_name = func_name + L" " + utf8ToWstring(typeid(T).name());
		static const wstring file_name = L".//Tests//" + DeleteCharacters(test_name) + L".txt";
		wfstream test_file(file_name, ios::out);
		test_file.precision(5);

		wcout << L"Test: " << test_name << L" started" << endl;
		for (size_t i = 0; i < iterations; ++i)
		{
			size_t size = 3 + i * multipler;
			T a(size, size);
			FillRandom(a);

			auto begin_t = std::chrono::high_resolution_clock::now();

			for (size_t m = 0; m < multipler; m++)
				a *= 253.324;

			auto end_t = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t);
			double time_s = (double)duration.count() / 1000;
			test_file << time_s << " ";

			wcout << L"Test: " << test_name << L" Time: " << setprecision(5) << time_s << " ";
			wcout << i << " " << size << L" " << (intmax_t(Sum(a)) % 10) << endl;
		}
		wcout << L"Test: " << test_name << L" finished" << endl;
		test_file.close();
	}
	template<typename T> void TestMul(size_t multipler = 10, size_t iterations = 50)
	{
		static const wstring func_name{ L"TestMul" };


		static const wstring test_name = func_name + L" " + utf8ToWstring(typeid(T).name());
		static const wstring file_name = L".//Tests//" + DeleteCharacters(test_name) + L".txt";
		wfstream test_file(file_name, ios::out);
		test_file.precision(5);

		wcout << L"Test: " << test_name << L" started" << endl;
		for (size_t i = 0; i < iterations; ++i)
		{
			size_t size = 3 + i * multipler;
			T a(size, size);
			T b(size, size);
			FillRandom(a);
			FillRandom(b);

			auto begin_t = std::chrono::high_resolution_clock::now();

			auto c = a * b;
			/*for (size_t m = 0; m < multipler; m++)
				auto c = c * b;*/

			auto end_t = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t);
			double time_s = (double)duration.count() / 1000;
			test_file << time_s << " ";

			wcout << L"Test: " << test_name << L" Time: " << setprecision(5) << time_s << " ";
			wcout << i << " " << size << L" " << (intmax_t(Sum(c)) % 10) << endl;
		}
		wcout << L"Test: " << test_name << L" finished" << endl;
		test_file.close();
	}

	void BenchMain()
	{
		//TestMulScalar<Matrix>();
		//TestMulScalar<MatrixOpenMP>();

		TestMul<Matrix>();
		TestMul<MatrixOpenMP>();
	}
}