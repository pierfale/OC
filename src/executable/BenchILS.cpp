#include "solver/ILSSolver.h"
#include "shared/Benchmark.h"


int main(int argc, const char** argv) {
	if(argc != 5) {
		std::cout << "usage : <problem_size> <input_file> <input_max_score> <output_file>" << std::endl;
		return EXIT_FAILURE;
	}


	Benchmark<1> bench(argv[4]);

	std::vector<const char*> perturbations = {"swap"/*, "insert", "exchange"*/};
	std::vector<const char*> stop = {"time"/*, "update"*/};
#define ARGS_NUMBER 7

	const char* bench_argv[ARGS_NUMBER];
	bench_argv[0] = argv[0];
	bench_argv[1] = argv[1];
	bench_argv[2] = argv[2];
	bench_argv[3] = "-perturbation";
	bench_argv[5] = "-stop";

	for(unsigned int perturbation_index = 0; perturbation_index < perturbations.size(); perturbation_index++) {
		bench_argv[4] = perturbations[perturbation_index];

		for(unsigned int stop_index = 0; stop_index < stop.size(); stop_index++) {
			bench_argv[6] = stop[stop_index];

			std::string name(bench_argv[4]);
			name += "_";
			name += bench_argv[6];

			bench.execute<ILSSolver, ILSProgramOption>(ARGS_NUMBER, bench_argv, argv[3], name);
		}


	}

	bench.save();

	return EXIT_SUCCESS;
}
