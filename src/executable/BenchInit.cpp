#include "solver/RandomSolver.h"
#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"

#include "shared/Benchmark.h"


int main(int argc, const char** argv) {
	if(argc != 5) {
		std::cout << "usage : <problem_size> <input_file> <input_max_score> <output_file>" << std::endl;
		return EXIT_FAILURE;
	}


	Benchmark<30> bench;

	bench.execute<RandomSolver, BasicProgramOption>(3, argv, argv[3], "random");
	bench.execute<EDDSolver, BasicProgramOption>(3, argv, argv[3], "edd");
	bench.execute<MDDSolver, BasicProgramOption>(3, argv, argv[3], "mdd");

	bench.save(argv[4]);

	return EXIT_SUCCESS;
}
