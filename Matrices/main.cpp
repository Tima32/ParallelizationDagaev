#include <iostream>
#include <iomanip>
#include "Matrix.hpp"
#include "MatrixOpenMP.hpp"
#include "Benchmark.hpp"

#include <random> // для std::random_device и std::mt19937
#include <chrono>

using namespace std;
using namespace PD;

int main()
{
	std::cout << std::fixed << std::setprecision(2);

# ifdef _OPENMP
	printf("Compiled by an OpenMP-compliant implementation.\n");
# endif
	
	PD::BENCH::BenchMain();

	return 0;
}