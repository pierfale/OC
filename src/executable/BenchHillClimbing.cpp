#include "solver/HillClimbingSolver.h"
#include "shared/Benchmark.h"


int main(int argc, const char** argv) {
	if(argc != 5) {
		std::cout << "usage : <problem_size> <input_file> <input_max_score> <output_file>" << std::endl;
		return EXIT_FAILURE;
	}


	Benchmark<30> bench;

	std::vector<const char*> inits = {/*"rnd", "edd", */"mdd"};
	std::vector<const char*> selects = {/*"first", */"best"};
	std::vector<const char*> neighborhoods = {"swap"/*, "insert", "exchange"*/};

#define ARGS_NUMBER 9

	const char* bench_argv[ARGS_NUMBER];
	bench_argv[0] = argv[0];
	bench_argv[1] = argv[1];
	bench_argv[2] = argv[2];
	bench_argv[3] = "-init";
	bench_argv[5] = "-select";
	bench_argv[7] = "-neighborhood";

	for(unsigned int init_index = 0; init_index < inits.size(); init_index++) {
		bench_argv[4] = inits[init_index];
		for(unsigned int select_index = 0; select_index < selects.size(); select_index++) {
			bench_argv[6] = selects[select_index];
			for(unsigned int neighborhood_index = 0; neighborhood_index < neighborhoods.size(); neighborhood_index++) {
				bench_argv[8] = neighborhoods[neighborhood_index];

				std::string name(bench_argv[4]);
				name += "_";
				name += bench_argv[6];
				name += "_";
				name += bench_argv[8];

				bench.execute<HillClimbingSolver, HillClimbingProgramOption>(ARGS_NUMBER, bench_argv, argv[3], name);
			}
		}

	}

	bench.save(argv[4]);

	return EXIT_SUCCESS;
}
