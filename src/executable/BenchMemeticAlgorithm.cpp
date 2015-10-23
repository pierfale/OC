#include "solver/MemeticAlgorithmSolver.h"
#include "shared/Benchmark.h"

int main(int argc, const char** argv) {
	if(argc != 5) {
		std::cout << "usage : <problem_size> <input_file> <input_max_score> <output_file>" << std::endl;
		return EXIT_FAILURE;
	}


	Benchmark<1> bench(argv[4]);

	std::vector<const char*> population = {"10", "25", "50", "100", "1000"};
	std::vector<const char*> generation = {"10", "25", "50", "100", "1000"};
	std::vector<const char*> creation = {"0.25", "0.5", "0.75", "1.0"};
	std::vector<const char*> mutation = {"0.0", "0.3", "0.6", "1.0"};
#define ARGS_NUMBER 11

	const char* bench_argv[ARGS_NUMBER];
	bench_argv[0] = argv[0];
	bench_argv[1] = argv[1];
	bench_argv[2] = argv[2];
	bench_argv[3] = "-population";
	bench_argv[5] = "-generation";
	bench_argv[7] = "-creation-rate";
	bench_argv[9] = "-mutation-rate";

	for(unsigned int  population_index = 0;  population_index <  population.size();  population_index++) {
		bench_argv[4] =  population[population_index];

		for(unsigned int generation_index = 0; generation_index < generation.size(); generation_index++) {
			bench_argv[6] = generation[generation_index];

			for(unsigned int creation_index = 0; creation_index < creation.size(); creation_index++) {
				bench_argv[8] = creation[creation_index];

				for(unsigned int mutation_index = 0; mutation_index < mutation.size(); mutation_index++) {
					bench_argv[10] = mutation[mutation_index];

					std::string name(bench_argv[4]);
					name += "_";
					name += bench_argv[6];
					name += "_";
					name += bench_argv[8];
					name += "_";
					name += bench_argv[10];

					bench.execute<MemeticAlgorithm, MemeticAlgorithmProgramOption>(ARGS_NUMBER, bench_argv, argv[3], name);
				}
			}
		}


	}

	bench.save();

	return EXIT_SUCCESS;
}
