#include "solver/ILSSolver.h"
#include "shared/Benchmark.h"


int main(int argc, const char** argv) {
	if(argc != 5) {
		std::cout << "usage : <problem_size> <input_file> <input_max_score> <output_file>" << std::endl;
		return EXIT_FAILURE;
	}


	Benchmark<1> bench;

	std::vector<const char*> perturbations = {"swap", "insert", "exchange"};
#define ARGS_NUMBER 5

	const char* bench_argv[ARGS_NUMBER];
	bench_argv[0] = argv[0];
	bench_argv[1] = argv[1];
	bench_argv[2] = argv[2];
	bench_argv[3] = "-perturbation";

	for(unsigned int perturbation_index = 0; perturbation_index < perturbations.size(); perturbation_index++) {
		bench_argv[4] = perturbations[perturbation_index];

		std::string name(bench_argv[4]);

		bench.execute<ILSSolver, ILSProgramOption>(ARGS_NUMBER, bench_argv, argv[3], name);

	}

	bench.save(argv[4]);

	return EXIT_SUCCESS;
}
